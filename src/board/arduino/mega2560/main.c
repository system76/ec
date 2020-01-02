#include <stdio.h>

#include <arch/gpio.h>
#include <arch/i2c_slave.h>
#include <arch/uart.h>
#include <board/battery.h>
#include <board/i2c.h>

void init(void) {
    uart_stdio_init(0, __CONSOLE_BAUD__);
    // Disable SPI
    SPCR = 0;
}

struct Gpio LED = GPIO(B, 7);

//TODO: .h file
void parallel_host(void);
void parallel_peripheral(void);
void parallel_spy(void);

int main(void) {
    init();

    gpio_set_dir(&LED, true);
    gpio_set(&LED, false);
    printf("Hello from System76 EC for the Arduino Mega 2560!\n");

    for (;;) {
        printf("Press a key to select parallel mode:\n");
        printf("  h = host, p = peripheral, s = spy\n");
        int c = getchar();
        if (c == 'h') {
            parallel_host();
        } else if (c == 'p') {
            parallel_peripheral();
        } else if (c == 's') {
            parallel_spy();
        }
    }
}
