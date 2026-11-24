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

int16_t peci_temp = 0;

// Update interval is 250ms, so average over 1s period.
// NOTE: PECI GetTemp() internally uses a moving average over 256ms.
static int16_t peci_temps[4] = { 0 };

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

void peci_read_temp(void) {
    peci_temps[0] = peci_temps[1];
    peci_temps[1] = peci_temps[2];
    peci_temps[2] = peci_temps[3];

    bool peci_on = peci_available();
    if (peci_on) {
        if (peci_get_temp(&peci_temps[3]) != 0) {
            peci_temps[3] = 0;
        }
    } else {
        peci_temps[3] = 0;
    }

    peci_temp = (peci_temps[0] + peci_temps[1] + peci_temps[2] + peci_temps[3]) /
        ARRAY_SIZE(peci_temps);
}
