#include <stdio.h>

#include <arch/gpio.h>
#include <arch/uart.h>

void init(void) {
    uart_stdio_init(0, __CONSOLE_BAUD__);
}

struct Gpio LED = GPIO(B, 5);

int main(void) {
    init();

    gpio_set_dir(&LED, true);
    gpio_set(&LED, false);
    printf("Hello from System76 EC for the Arduino Uno!\n");
    for (;;) {
        int c = getchar();
        if (c == '\r') {
            putchar('\n');
        } else if (c > 0) {
            putchar(c);
        }
    }
}
