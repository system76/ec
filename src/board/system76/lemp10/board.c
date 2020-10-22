// SPDX-License-Identifier: GPL-3.0-only

#include <board/board.h>
#include <board/espi.h>
#include <board/gpio.h>
#include <board/power.h>
#include <common/debug.h>

extern uint8_t main_cycle;

void board_init(void) {
    espi_init();

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
    // Assert SMI#, SCI#, and SWI#
    gpio_set(&SCI_N, true);
    gpio_set(&SMI_N, true);
    gpio_set(&SWI_N, true);
}

void board_on_ac(bool ac) { /* Fix unused variable */ ac = ac; }

struct Gpio __code CPU_C10_GATE_N = GPIO(F, 7);
struct Gpio __code SLP_S0 = GPIO(J, 0);
struct Gpio __code VCCIN_AUX_PG = GPIO(G, 0);

#define GPIO_DEBUG_CHANGED(G) { \
    static int last_ ## G = -1; \
    int new_ ## G = (int)gpio_get(&G); \
    if (new_ ## G != last_ ## G) { \
        DEBUG( \
            "%S: %d changed to %d\n", \
            #G, \
            last_ ## G, \
            new_ ## G \
        ); \
        last_ ## G = new_ ## G; \
    } \
}

void board_event(void) {
    espi_event();

    GPIO_DEBUG_CHANGED(ALL_SYS_PWRGD);
    GPIO_DEBUG_CHANGED(BUF_PLT_RST_N);
    GPIO_DEBUG_CHANGED(CPU_C10_GATE_N);
    GPIO_DEBUG_CHANGED(SLP_S0);
    GPIO_DEBUG_CHANGED(SLP_SUS_N);
    GPIO_DEBUG_CHANGED(SUSB_N_PCH);
    GPIO_DEBUG_CHANGED(SUSC_N_PCH);
    GPIO_DEBUG_CHANGED(VCCIN_AUX_PG);
    GPIO_DEBUG_CHANGED(VR_ON);
}
