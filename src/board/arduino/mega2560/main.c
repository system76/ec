#include <stdio.h>

#include <arch/gpio.h>
#include <arch/uart.h>
#include <board/battery.h>
#include <board/i2c.h>

void init(void) {
    uart_stdio_init(0, __CONSOLE_BAUD__);
    i2c_init(50000);
}

struct Gpio LED = GPIO(B, 7);

int main(void) {
    init();

    gpio_set_dir(&LED, true);
    gpio_set(&LED, false);
    printf("Hello from System76 EC for the Arduino Mega 2560!\n");

    battery_debug();

    for (;;) {
        int c = getchar();
        if (c == '\r') {
            putchar('\n');
            battery_debug();
        } else if (c > 0) {
            putchar(c);
        }
    }
}
