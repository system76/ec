#include <stdio.h>

#include <arch/uart.h>

void init(void) {
    uart_stdio_init(0, 9600);
}

void main(void) {
    init();

    printf("Hello from System76 EC for the Arduino Mega 2560!\n");
    for (;;) {}
}
