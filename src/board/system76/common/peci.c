// SPDX-License-Identifier: GPL-3.0-only

// PECI information can be found here:
// https://www.intel.com/content/dam/www/public/us/en/documents/design-guides/core-i7-lga-2011-guide.pdf

#include <arch/time.h>
#include <board/espi.h>
#include <board/fan.h>
#include <board/gpio.h>
#include <board/peci.h>
#include <board/power.h>
#include <common/debug.h>
#include <common/macro.h>
#include <ec/gpio.h>
#include <ec/pwm.h>

bool peci_on = false;
int16_t peci_temp = 0;

// Update interval is 250ms, so average over 1s period
static int16_t peci_temps[4] = { 0 };

// Tjunction = 100C for i7-8565U (and probably the same for all WHL-U)
#define T_JUNCTION ((int16_t)100)

// Maximum OOB channel response time in ms
#define PECI_ESPI_TIMEOUT 10

// Returns true if peci is available
bool peci_available(void) {
    // Ensure power state is up to date
    update_power_state();

    // Power state must be S0 for PECI to be useful
    if (power_state != POWER_STATE_S0)
        return false;

#if CONFIG_BUS_ESPI
    // Currently waiting for host reset, PECI is not available
    if (espi_host_reset)
        return false;

    // If VW_PLTRST_N virtual wire is not VWS_HIGH, PECI is not available
    // This is because the CPU has not yet exited reset
    if (vw_get(&VW_PLTRST_N) != VWS_HIGH)
        return false;

    // If VW_HOST_C10 virtual wire is VWS_HIGH, PECI will wake the CPU
    //TODO: wake CPU every 8 seconds following Intel recommendation?
    return vw_get(&VW_HOST_C10) != VWS_HIGH;
#else
    // PECI is available if PLTRST# is high
    return gpio_get(&BUF_PLT_RST_N);
#endif // CONFIG_BUS_ESPI
}

#if CONFIG_PECI_OVER_ESPI

void peci_init(void) {}

// Returns true on success, false on error
bool peci_get_temp(int16_t *const data) {
    //TODO: Wait for completion?
    // Clear upstream status
    ESUCTRL0 = ESUCTRL0;
    // Clear OOB status
    ESOCTRL0 = ESOCTRL0;

    // Set upstream cycle type
    ESUCTRL1 = ESUCTRL1_OOB;
    // Set upstream tag / length[11:8]
    ESUCTRL2 = 0;
    // Set upstream length [7:0] (size of PECI data plus 3)
    ESUCTRL3 = 8;

    // Destination address (0x10 is PCH, left shifted by one)
    UDB[0] = 0x10 << 1;
    // Command code (0x01 is PECI)
    UDB[1] = 0x01;
    // Set byte count
    UDB[2] = 5;
    // Set source address (0x0F is EC, left shifted by one, or with 1)
    UDB[3] = (0x0F << 1) | 1;
    // PECI target address (0x30 is default)
    UDB[4] = 0x30;
    // PECI write length
    UDB[5] = 1;
    // PECI read length
    UDB[6] = 2;
    // PECI command
    UDB[7] = PECI_CMD_GET_TEMP;

    // Set upstream enable
    ESUCTRL0 |= ESUCTRL0_ENABLE;
    // Set upstream go
    ESUCTRL0 |= ESUCTRL0_GO;

    // Wait until upstream done
    systick_t start = time_get();
    while (!(ESUCTRL0 & ESUCTRL0_DONE)) {
        if ((time_get() - start) >= PECI_ESPI_TIMEOUT) {
            DEBUG("peci_get_temp: upstream timeout\n");
            return false;
        }
    }
    // Clear upstream done status
    ESUCTRL0 = ESUCTRL0_DONE;

    // Wait for response
    //TODO: do this asynchronously to avoid delays?
    start = time_get();
    while (!(ESOCTRL0 & ESOCTRL0_STATUS)) {
        if ((time_get() - start) >= PECI_ESPI_TIMEOUT) {
            DEBUG("peci_get_temp: response timeout\n");
            return false;
        }
    }

    // Read response length
    uint8_t len = ESOCTRL4;
    if (len >= 7) {
        //TODO: verify packet type, handle PECI status

        // Received enough data for temperature
        uint8_t low = PUTOOBDB[5];
        uint8_t high = PUTOOBDB[6];
        int16_t offset = (((int16_t)high << 8) | (int16_t)low);
        *data = T_JUNCTION + (offset >> 6);

        // Clear PUT_OOB status
        ESOCTRL0 = ESOCTRL0_STATUS;

        return true;
    } else {
        // Did not receive enough data
        DEBUG("peci_get_temp: len %d < 7\n", len);
        // Clear PUT_OOB status
        ESOCTRL0 = ESOCTRL0_STATUS;
        return false;
    }
}

// Returns positive completion code on success, negative completion code or
// negative (0x1000 | status register) on PECI hardware error
int16_t peci_wr_pkg_config(uint8_t index, uint16_t param, uint32_t data) {
    //TODO: Wait for completion?
    // Clear upstream status
    ESUCTRL0 = ESUCTRL0;
    // Clear OOB status
    ESOCTRL0 = ESOCTRL0;

    // Set upstream cycle type
    ESUCTRL1 = ESUCTRL1_OOB;
    // Set upstream tag / length[11:8]
    ESUCTRL2 = 0;
    // Set upstream length [7:0] (size of PECI data plus 3)
    ESUCTRL3 = 16;

    // Destination address (0x10 is PCH, left shifted by one)
    UDB[0] = 0x10 << 1;
    // Command code (0x01 is PECI)
    UDB[1] = 0x01;
    // Set byte count
    UDB[2] = 13;
    // Set source address (0x0F is EC, left shifted by one, or with 1)
    UDB[3] = (0x0F << 1) | 1;
    // PECI target address (0x30 is default)
    UDB[4] = 0x30;
    // PECI write length
    UDB[5] = 10;
    // PECI read length
    UDB[6] = 1;
    // PECI command
    UDB[7] = PECI_CMD_WR_PKG_CONFIG;

    // Write host ID
    UDB[8] = 0;
    // Write index
    UDB[9] = index;
    // Write param
    UDB[10] = (uint8_t)param;
    UDB[11] = (uint8_t)(param >> 8);
    // Write data
    UDB[12] = (uint8_t)data;
    UDB[13] = (uint8_t)(data >> 8);
    UDB[14] = (uint8_t)(data >> 16);
    UDB[15] = (uint8_t)(data >> 24);

    // Set upstream enable
    ESUCTRL0 |= ESUCTRL0_ENABLE;
    // Set upstream go
    ESUCTRL0 |= ESUCTRL0_GO;

    // Wait until upstream done
    systick_t start = time_get();
    while (!(ESUCTRL0 & ESUCTRL0_DONE)) {
        DEBUG("peci_wr_pkg_config: wait upstream\n");
        if ((time_get() - start) >= PECI_ESPI_TIMEOUT) {
            DEBUG("peci_wr_pkg_config: upstream timeout\n");
            return false;
        }
    }
    // Clear upstream done status
    ESUCTRL0 = ESUCTRL0_DONE;

    // Wait for response
    //TODO: do this asynchronously to avoid delays?
    start = time_get();
    while (!(ESOCTRL0 & ESOCTRL0_STATUS)) {
        DEBUG("peci_wr_pkg_config: wait response\n");
        if ((time_get() - start) >= PECI_ESPI_TIMEOUT) {
            DEBUG("peci_wr_pkg_config: response timeout\n");
            return false;
        }
    }

    // Read response length
    uint8_t len = ESOCTRL4;
    if (len >= 6) {
        //TODO: verify packet type, handle PECI status

        // Received enough data for status code
        int16_t cc = (int16_t)PUTOOBDB[5];

        // Clear PUT_OOB status
        ESOCTRL0 = ESOCTRL0_STATUS;

        if (cc & 0x80) {
            return -cc;
        } else {
            return cc;
        }
    } else {
        // Did not receive enough data
        DEBUG("peci_wr_pkg_config: len %d < 6\n", len);
        // Clear PUT_OOB status
        ESOCTRL0 = ESOCTRL0_STATUS;
        return -0x1000;
    }
}

#else // CONFIG_PECI_OVER_ESPI

// Legacy PECI implementation; requires a dedicated PECI pin connected to the
// PCH and EC (H_PECI).

void peci_init(void) {
    // Allow PECI pin to be used
    GCR2 |= BIT(4);

    // Set frequency to 1MHz
    HOCTL2R = 0x01;
    // Set VTT to 1V
    PADCTLR = 0x02;
}

// Returns true on success, false on error
bool peci_get_temp(int16_t *const data) {
    // Wait for any in-progress transaction to complete
    systick_t start = time_get();
    while (HOSTAR & BIT(0)) {
        if ((time_get() - start) >= PECI_ESPI_TIMEOUT) {
            DEBUG("%s: host timeout\n", __func__);
            return false;
        }
    }

    // Clear status
    HOSTAR = HOSTAR;

    // Enable PECI, clearing data fifo's
    HOCTLR = BIT(5) | BIT(3);
    // Set address to default
    HOTRADDR = 0x30;
    // Set write length
    HOWRLR = 1;
    // Set read length
    HORDLR = 2;
    // Set command
    HOCMDR = PECI_CMD_GET_TEMP;
    // Start transaction
    HOCTLR |= 1;

    // Wait for command completion
    start = time_get();
    while (!(HOSTAR & BIT(1))) {
        if ((time_get() - start) >= PECI_ESPI_TIMEOUT) {
            DEBUG("%s: command timeout\n", __func__);
            return false;
        }
    }

    uint8_t status = HOSTAR;
    if (status & 0xEC) {
        ERROR("peci_get_temp: hardware error: 0x%02X\n", status);
        // Clear status
        HOSTAR = HOSTAR;
        return false;
    }

    // Read two byte temperature data if finished successfully
    uint8_t low = HORDDR;
    uint8_t high = HORDDR;
    int16_t offset = (((int16_t)high << 8) | (int16_t)low);
    *data = T_JUNCTION + (offset >> 6);

    // Clear status
    HOSTAR = HOSTAR;
    return true;
}

// Returns positive completion code on success, negative completion code or
// negative (0x1000 | status register) on PECI hardware error
int16_t peci_wr_pkg_config(uint8_t index, uint16_t param, uint32_t data) {
    // Wait for any in-progress transaction to complete
    systick_t start = time_get();
    while (HOSTAR & BIT(0)) {
        if ((time_get() - start) >= PECI_ESPI_TIMEOUT) {
            DEBUG("%s: host timeout\n", __func__);
            return false;
        }
    }

    // Clear status
    HOSTAR = HOSTAR;

    // Enable PECI, clearing data fifo's, enable AW_FCS
    HOCTLR = BIT(5) | BIT(3) | BIT(1);
    // Set address to default
    HOTRADDR = 0x30;
    // Set write length
    HOWRLR = 10;
    // Set read length
    HORDLR = 1;
    // Set command
    HOCMDR = PECI_CMD_WR_PKG_CONFIG;

    // Write host ID
    HOWRDR = 0;
    // Write index
    HOWRDR = index;
    // Write param
    HOWRDR = (uint8_t)param;
    HOWRDR = (uint8_t)(param >> 8);
    // Write data
    HOWRDR = (uint8_t)data;
    HOWRDR = (uint8_t)(data >> 8);
    HOWRDR = (uint8_t)(data >> 16);
    HOWRDR = (uint8_t)(data >> 24);

    // Start transaction
    HOCTLR |= 1;

    // Wait for command completion
    start = time_get();
    while (!(HOSTAR & BIT(1))) {
        if ((time_get() - start) >= PECI_ESPI_TIMEOUT) {
            DEBUG("%s: command timeout\n", __func__);
            return false;
        }
    }

    uint8_t status = HOSTAR;
    if (status & 0xEC) {
        ERROR("peci_wr_pkg_config: hardware error: 0x%02X\n", status);
        // Clear status
        HOSTAR = HOSTAR;
        return -(0x1000 | status);
    }

    uint8_t cc = HORDDR;

    // Clear status
    HOSTAR = HOSTAR;

    if (cc == 0x40) {
        TRACE("peci_wr_pkg_config: command successful\n");
        return cc;
    }

    ERROR("peci_wr_pkg_config: command error: 0x%02X\n", cc);
    return -((int16_t)cc);
}

#endif // CONFIG_PECI_OVER_ESPI

void peci_read_temp(void) {
    peci_temps[0] = peci_temps[1];
    peci_temps[1] = peci_temps[2];
    peci_temps[2] = peci_temps[3];

    peci_on = peci_available();
    if (peci_on) {
        if (!peci_get_temp(&peci_temps[3])) {
            peci_temps[3] = 0;
        }
    } else {
        peci_temps[3] = 0;
    }

    peci_temp = (peci_temps[0] + peci_temps[1] + peci_temps[2] + peci_temps[3]) /
        ARRAY_SIZE(peci_temps);
}
