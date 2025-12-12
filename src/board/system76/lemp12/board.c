// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2023 System76, Inc.

#include <board/battery.h>
#include <board/board.h>
#include <board/espi.h>
#include <board/gpio.h>
#include <ec/ec.h>

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
    // Assert SMI#, SCI#, and SWI#
    gpio_set(&SCI_N, true);
    gpio_set(&SMI_N, true);
    gpio_set(&SWI_N, true);
}

void board_event(void) {
    espi_event();

    ec_read_post_codes();

    // Hack to drain LDO_3V3 capacitors
    if (!gpio_get(&XLP_OUT)) {
        // These are pulled up by LDO_3V3, pull them low
        // SMB_CLK_EC
        GPCRC1 = GPIO_OUT;
        // SMB_DATA_EC
        GPCRC2 = GPIO_OUT;
        // SMB_CLK_EC, SMB_DATA_EC
        GPDRC &= ~(BIT(1) | BIT(2));
    }
}
