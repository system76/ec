// SPDX-License-Identifier: GPL-3.0-only

#include <board/fan.h>
#include <board/gpio.h>
#include <board/peci.h>
#include <board/power.h>
#include <common/debug.h>
#include <common/macro.h>
#include <ec/espi.h>
#include <ec/gpio.h>
#include <ec/pwm.h>

#ifndef USE_PECI_OVER_ESPI
#define USE_PECI_OVER_ESPI 0
#endif

// Fan speed is the lowest requested over HEATUP seconds
#ifndef BOARD_HEATUP
#define BOARD_HEATUP 4
#endif

static uint8_t FAN_HEATUP[BOARD_HEATUP] = { 0 };

// Fan speed is the highest HEATUP speed over COOLDOWN seconds
#ifndef BOARD_COOLDOWN
#define BOARD_COOLDOWN 10
#endif

static uint8_t FAN_COOLDOWN[BOARD_COOLDOWN] = { 0 };

// Tjunction = 100C for i7-8565U (and probably the same for all WHL-U)
#define T_JUNCTION 100

bool peci_on = false;
int16_t peci_temp = 0;

#define PECI_TEMP(X) ((int16_t)(X))

// clang-format off
#define FAN_POINT(T, D) { .temp = PECI_TEMP(T), .duty = PWM_DUTY(D) }
// clang-format on

// Fan curve with temperature in degrees C, duty cycle in percent
static struct FanPoint __code FAN_POINTS[] = {
#ifdef BOARD_FAN_POINTS
    BOARD_FAN_POINTS
#else
    FAN_POINT(70, 40),
    FAN_POINT(75, 50),
    FAN_POINT(80, 60),
    FAN_POINT(85, 65),
    FAN_POINT(90, 65)
#endif
};

static struct Fan __code FAN = {
    .points = FAN_POINTS,
    .points_size = ARRAY_SIZE(FAN_POINTS),
    .heatup = FAN_HEATUP,
    .heatup_size = ARRAY_SIZE(FAN_HEATUP),
    .cooldown = FAN_COOLDOWN,
    .cooldown_size = ARRAY_SIZE(FAN_COOLDOWN),
    .interpolate = SMOOTH_FANS != 0,
};

#if USE_PECI_OVER_ESPI

void peci_init(void) {}

// Returns true on success, false on error
bool peci_get_temp(int16_t * data) {
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
    // PECI command (0x01 = GetTemp)
    UDB[7] = 0x01;

    // Set upstream enable
    ESUCTRL0 |= ESUCTRL0_ENABLE;
    // Set upstream go
    ESUCTRL0 |= ESUCTRL0_GO;

    // Wait until upstream done
    while (!(ESUCTRL0 & ESUCTRL0_DONE)) {}

    // Wait for response
    //TODO: do this asynchronously to avoid delays?
    while (!(ESOCTRL0 & ESOCTRL0_STATUS)) {}

    // Read response length
    uint8_t len = ESOCTRL4;
    if (len >= 7) {
        //TODO: verify packet type, handle PECI status

        // Received enough data for temperature
        uint8_t low = PUTOOBDB[5];
        uint8_t high = PUTOOBDB[6];
        *data = (((int16_t)high << 8) | (int16_t)low);
        return true;
    } else {
        // Did not receive enough data
        return false;
    }
}

// Returns positive completion code on success, negative completion code or
// negative (0x1000 | status register) on PECI hardware error
int16_t peci_wr_pkg_config(uint8_t index, uint16_t param, uint32_t data) {
    //TODO: IMPLEMENT THIS STUB
    DEBUG("peci_wr_pkg_config %02X, %04X, %08X\n", index, param, data);
    index = index;
    param = param;
    data = data;
    return 0;
}

#else // USE_PECI_OVER_ESPI

void peci_init(void) {
    // Allow PECI pin to be used
    GCR2 |= BIT(4);

    // Set frequency to 1MHz
    HOCTL2R = 0x01;
    // Set VTT to 1V
    PADCTLR = 0x02;
}

// Returns true on success, false on error
bool peci_get_temp(int16_t * data) {
    // Wait for completion
    while (HOSTAR & 1) {}
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
    HOCMDR = 1;
    // Start transaction
    HOCTLR |= 1;

    // Wait for completion
    while (HOSTAR & 1) {}

    uint8_t status = HOSTAR;
    if (status & BIT(1)) {
        // Read two byte temperature data if finished successfully
        uint8_t low = HORDDR;
        uint8_t high = HORDDR;
        *data = (((int16_t)high << 8) | (int16_t)low);
        return true;
    } else {
        return false;
    }
}

// Returns positive completion code on success, negative completion code or
// negative (0x1000 | status register) on PECI hardware error
int16_t peci_wr_pkg_config(uint8_t index, uint16_t param, uint32_t data) {
    // Wait for completion
    while (HOSTAR & 1) {}
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
    HOCMDR = 0xA5;

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

    // Wait for completion
    while (HOSTAR & 1) {}

    int16_t status = (int16_t)HOSTAR;
    if (status & BIT(1)) {
        int16_t cc = (int16_t)HORDDR;
        if (cc & 0x80) {
            return -cc;
        } else {
            return cc;
        }
    } else {
        return -(0x1000 | status);
    }
}

#endif // USE_PECI_OVER_ESPI

// PECI information can be found here: https://www.intel.com/content/dam/www/public/us/en/documents/design-guides/core-i7-lga-2011-guide.pdf
uint8_t peci_get_fan_duty(void) {
    uint8_t duty;

#if CONFIG_BUS_ESPI
    // Use PECI if CPU is not in C10 sleep state
    // HOST_C10 virtual wire is high when CPU is in C10 sleep state
    peci_on = !vw_get(&VW_HOST_C10);
#else // CONFIG_BUS_ESPI
    // Use PECI if in S0 state
    peci_on = power_state == POWER_STATE_S0;
#endif // CONFIG_BUS_ESPI

    if (peci_on) {
        int16_t peci_offset = 0;
        if (peci_get_temp(&peci_offset)) {
            // Use result if finished successfully
            peci_temp = PECI_TEMP(T_JUNCTION) + (peci_offset >> 6);
            duty = fan_duty(&FAN, peci_temp);
        } else {
            // Default to 50% if there is an error
            peci_temp = 0;
            duty = PWM_DUTY(50);
        }
    } else {
        // Turn fan off if not in S0 state
        peci_temp = 0;
        duty = PWM_DUTY(0);
    }

    if (peci_on && fan_max) {
        // Override duty if fans are manually set to maximum
        duty = PWM_DUTY(100);
    } else {
        // Apply heatup and cooldown filters to duty
        duty = fan_heatup(&FAN, duty);
        duty = fan_cooldown(&FAN, duty);
    }

    TRACE("PECI temp=%d\n", peci_temp);
    return duty;
}
