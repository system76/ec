#include <arch/time.h>
#include <board/battery.h>
#include <board/board.h>
#include <board/dgpu.h>
#include <board/gpio.h>
#include <board/kbc.h>
#include <board/peci.h>
#include <board/power.h>
#include <common/debug.h>

extern uint8_t main_cycle;

void board_init(void) {
    // Allow CPU to boot
    gpio_set(&SB_KBCRST_N, true);
    // Allow backlight to be turned on
    gpio_set(&BKL_EN, true);
    // Enable camera
    gpio_set(&CCD_EN, true);
    // Enable wireless
    gpio_set(&BT_EN, true);
    gpio_set(&WLAN_EN, true);
    gpio_set(&WLAN_PWR_EN, true);
    // Enable USB port power?
    gpio_set(&USB_PWR_EN_N, false);
    // Assert SMI#, SCI#, and SWI#
    gpio_set(&SCI_N, true);
    gpio_set(&SMI_N, true);
    gpio_set(&SWI_N, true);

    dgpu_init();
}

// Set PL4 using PECI
static int set_power_limit(uint8_t watts) {
    return peci_wr_pkg_config(
        60, // index
        0, // param
        ((uint32_t)watts) * 8
    );
}

void board_event(void) {
    bool acin = !gpio_get(&ACIN_N);
    gpio_set(&AC_PRESENT, acin);

    static uint8_t last_power_limit = 0;
    if (power_state == POWER_STATE_S0) {
        uint8_t power_limit = acin ? POWER_LIMIT_AC : POWER_LIMIT_DC;
        if (power_limit != last_power_limit) {
            int res = set_power_limit(power_limit);
            DEBUG("set_power_limit %d = %d\n", power_limit, res);
            if (res >= 0) {
                last_power_limit = power_limit;
            } else {
                ERROR("set_power_limit failed: %X\n", -res);
            }
        }
    } else {
        last_power_limit = 0;
    }

    if (main_cycle == 0) {
        // Battery charging LED is inverted on the oryp6
        if (acin) {
            // Discharging (no AC adapter)
            gpio_set(&LED_BAT_CHG_N, true);
            gpio_set(&LED_BAT_FULL, false);
        } else if (battery_current == 0) {
            // Fully charged
            // TODO: turn off charger
            gpio_set(&LED_BAT_CHG_N, true);
            gpio_set(&LED_BAT_FULL, true);
        } else {
            // Charging
            // TODO: detect no battery connected
            gpio_set(&LED_BAT_CHG_N, false);
            gpio_set(&LED_BAT_FULL, false);
        }

        if (power_state == POWER_STATE_S0 || power_state == POWER_STATE_S3 || power_state == POWER_STATE_DS3) {
            // System is on
        } else if (!acin) {
            // Power off VDD3 if system should be off
            gpio_set(&XLP_OUT, 0);
        }

        static uint32_t last_time = 0;
        uint32_t time = time_get();
        // Only run the following once a second
        if (last_time > time || (time - last_time) >= 1000) {
            last_time = time;

            // Updates discrete GPU fan status and temps
            dgpu_event();
        }
    }
}
