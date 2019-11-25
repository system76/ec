#include <stdio.h>

#include <arch/gpio.h>
#include <arch/i2c_slave.h>
#include <arch/uart.h>
#include <board/battery.h>
#include <board/i2c.h>

void init(void) {
    uart_stdio_init(0, __CONSOLE_BAUD__);
    i2c_init(50000);
}

static void i2c_slave_new() {}

static void i2c_slave_recv(uint8_t data) {
    printf("%c", data);
}

static uint8_t i2c_slave_send() {
    return 0;
}

struct Gpio LED = GPIO(B, 5);

int main(void) {
    init();

    gpio_set_dir(&LED, true);
    gpio_set(&LED, false);
    printf("Hello from System76 EC for the Arduino Uno!\n");

    battery_debug();

    for (;;) {
        i2c_slave_init(0x76, i2c_slave_new, i2c_slave_recv, i2c_slave_send);
        int c = getchar();
        i2c_slave_stop();
        if (c == '\r') {
            putchar('\n');
            battery_debug();
        } else if (c > 0) {
            putchar(c);
        }
    }
}
