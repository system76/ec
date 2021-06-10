// SPDX-License-Identifier: GPL-3.0-only

#include <board/battery.h>
#include <board/board.h>
#include <board/dgpu.h>
#include <board/espi.h>
#include <board/gctrl.h>
#include <board/gpio.h>
#include <board/peci.h>
#include <board/power.h>
#include <common/debug.h>
#include <ec/ec.h>

extern uint8_t main_cycle;

void board_init(void) {
    espi_init();

    // Make sure charger is in off state, also enables PSYS
    battery_charger_disable();

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
    // Enable right USB port
    gpio_set(&USB_PWR_EN_N, false);
    // Assert SMI# and SWI#
    gpio_set(&SMI_N, true);
    gpio_set(&SWI_N, true);
}

#if HAVE_DGPU
// Set PL4 using PECI
static int set_power_limit(uint8_t watts) {
    return peci_wr_pkg_config(
        60, // index
        0, // param
        ((uint32_t)watts) * 8
    );
}

void board_on_ac(bool ac) {
    uint8_t power_limit = ac ? POWER_LIMIT_AC : POWER_LIMIT_DC;
    // Retry, timeout errors happen occasionally
    for (int i = 0; i < 16; i++) {
        int res = set_power_limit(power_limit);
        DEBUG("set_power_limit %d = %d\n", power_limit, res);
        if (res == 0x40) {
            break;
        } else if (res < 0) {
            ERROR("set_power_limit failed: 0x%02X\n", -res);
        } else {
            ERROR("set_power_limit unknown response: 0x%02X\n", res);
        }
    }
}
#else // HAVE_DGPU
void board_on_ac(bool ac) { /* Fix unused variable */ ac = ac; }
#endif // HAVE_DGPU

void board_event(void) {
#if HAVE_DGPU
    static bool last_power_limit_ac = true;
    // We don't use power_state because the latency needs to be low
    if (gpio_get(&CPU_C10_GATE_N)) {
        bool ac = !gpio_get(&ACIN_N);
        if (last_power_limit_ac != ac) {
            board_on_ac(ac);
            last_power_limit_ac = ac;
        }
    } else {
        last_power_limit_ac = true;
    }
#endif // HAVE_DGPU

    espi_event();

    ec_read_post_codes();
}
