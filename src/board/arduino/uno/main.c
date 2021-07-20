// SPDX-License-Identifier: GPL-3.0-only

#include <stdio.h>

#include <arch/arch.h>
#include <arch/delay.h>
#include <arch/gpio.h>
#include <arch/time.h>
#include <arch/uart.h>

void init(void) {
    arch_init();
    uart_stdio_init(0, __CONSOLE_BAUD__);
}

struct Gpio LED = GPIO(B, 5);

int main(void) {
    init();

    gpio_set_dir(&LED, true);
    gpio_set(&LED, false);

    for (;;) {
        printf("%ld ms\r\n", time_get() / 1000);
        delay_ms(1000);
        gpio_set(&LED, !gpio_get(&LED));
    }
}
