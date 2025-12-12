// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2019 System76, Inc.

#include "gpio.h"
#include "uart.h"
#include <stdio.h>

void init(void) {
    uart_stdio_init(0, __CONSOLE_BAUD__);
}

struct Gpio LED = GPIO(B, 7);

//TODO: .h file
int parallel_main(void);

int main(void) {
    init();

    gpio_set_dir(&LED, true);
    gpio_set(&LED, false);

    parallel_main();

    // If parallel_main exits with an error, wait for reset
    for (;;) {}
}
