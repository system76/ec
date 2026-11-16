// SPDX-License-Identifier: GPL-3.0-only

// PECI information can be found here:
// https://www.intel.com/content/dam/www/public/us/en/documents/design-guides/core-i7-lga-2011-guide.pdf

#include <app/peci.h>
#include <app/espi.h>
#include <app/fan.h>
#include <app/power.h>
#include <arch/time.h>
#include <board/gpio.h>
#include <common/debug.h>
#include <common/macro.h>
#include <ec/gpio.h>
#include <ec/pwm.h>

bool peci_on = false;
int16_t peci_temp = 0;

// Update interval is 250ms, so average over 1s period.
// NOTE: PECI GetTemp() internally uses a moving average over 256ms.
static int16_t peci_temps[4] = { 0 };

#define PECI_CPU_ADDR   0x30

// PECI completion codes
#define PECI_CC_RSP_SUCCESS             0x40
#define PECI_CC_RSP_TIMEOUT             0x80
#define PECI_CC_RSP_OUT_OF_RESOURCES    0x81
#define PECI_CC_RSP_LOW_POWER_STATE     0x82
#define PECI_CC_RSP_INVALID_REQUEST     0x90
#define PECI_CC_RSP_LOGIC_ERROR         0x91

// Ping
#define PECI_PING_WR_LEN        0
#define PECI_PING_RD_LEN        0
// GetDIB
#define PECI_GET_DIB_WR_LEN     1
#define PECI_GET_DIB_RD_LEN     8
// GetTemp
#define PECI_GET_TEMP_WR_LEN    1
#define PECI_GET_TEMP_RD_LEN    2
// WrPkgConfig
#define PECI_WR_PKG_WR_LEN_1    7
#define PECI_WR_PKG_WR_LEN_2    8
#define PECI_WR_PKG_WR_LEN_4    10
#define PECI_WR_PKG_RD_LEN      1
// RdPkgConfig
#define PECI_RD_PKG_WR_LEN      5
#define PECI_RD_PKG_RD_LEN_1    2
#define PECI_RD_PKG_RD_LEN_2    3
#define PECI_RD_PKG_RD_LEN_4    5
// RdIAMSR
#define PECI_RD_IAMSR_WR_LEN    5
#define PECI_RD_IAMSR_RD_LEN_1  2
#define PECI_RD_IAMSR_RD_LEN_2  3
#define PECI_RD_IAMSR_RD_LEN_4  5
#define PECI_RD_IAMSR_RD_LEN_8  9

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

// Cycle type + Tag + Length bytes
#define OOB_PACKET_HDR_LEN 3

// Command code
#define OOB_CMD_PECI    0x01
// Source/Dest slave addresses
#define OOB_EC_ADDR     0x0F
#define OOB_PCH_ADDR    0x10

#define OOB_DEST(addr)  (((addr) << 1) | 0)
#define OOB_SRC(addr)   (((addr) << 1) | 1)

void peci_init(void) {}

// Returns true on success, false on error
bool peci_get_temp(int16_t *const data) {
    //TODO: Wait for completion?
    // Clear upstream status
    ESUCTRL0 = ESUCTRL0;
    // Clear OOB status
    ESOCTRL0 = ESOCTRL0;

    // eSPI cycle type
    ESUCTRL1 = ESUCTRL1_OOB;
    // Tag [3:0] / Length [11:8]
    ESUCTRL2 = 0;
    // Length [7:0]
    ESUCTRL3 = 5 + OOB_PACKET_HDR_LEN;

    // OOB destination address
    UDB[0] = OOB_DEST(OOB_PCH_ADDR);
    // OOB command code
    UDB[1] = OOB_CMD_PECI;
    // Byte count
    UDB[2] = 5;
    // OOB source address
    UDB[3] = OOB_SRC(OOB_EC_ADDR);
    // PECI target address
    UDB[4] = PECI_CPU_ADDR;
    // PECI write length
    UDB[5] = PECI_GET_TEMP_WR_LEN;
    // PECI read length
    UDB[6] = PECI_GET_TEMP_RD_LEN;
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

    // eSPI cycle type
    ESUCTRL1 = ESUCTRL1_OOB;
    // Tag [3:0] / Length [11:8]
    ESUCTRL2 = 0;
    // Length [7:0]
    ESUCTRL3 = 13 + OOB_PACKET_HDR_LEN;

    // OOB destination address
    UDB[0] = OOB_DEST(OOB_PCH_ADDR);
    // OOB command code
    UDB[1] = OOB_CMD_PECI;
    // Byte count
    UDB[2] = 13;
    // OOB source address
    UDB[3] = OOB_SRC(OOB_EC_ADDR);
    // PECI target address
    UDB[4] = PECI_CPU_ADDR;
    // PECI write length
    UDB[5] = PECI_WR_PKG_WR_LEN_4;
    // PECI read length
    UDB[6] = PECI_WR_PKG_RD_LEN;
    // PECI command
    UDB[7] = PECI_CMD_WR_PKG_CONFIG;
    // Host ID [7:1] & Retry [0]
    UDB[8] = 0;
    // Index
    UDB[9] = index;
    // Param
    UDB[10] = (uint8_t)param;
    UDB[11] = (uint8_t)(param >> 8);
    // Data
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

        if (cc & BIT(7)) {
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
    // Set address
    HOTRADDR = PECI_CPU_ADDR;
    // Set write length
    HOWRLR = PECI_GET_TEMP_WR_LEN;
    // Set read length
    HORDLR = PECI_GET_TEMP_RD_LEN;
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
    // Set address
    HOTRADDR = PECI_CPU_ADDR;
    // Set write length
    HOWRLR = PECI_WR_PKG_WR_LEN_4;
    // Set read length
    HORDLR = PECI_WR_PKG_RD_LEN;
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

    if (cc == PECI_CC_RSP_SUCCESS) {
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
