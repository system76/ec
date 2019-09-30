#include <stdio.h>

#include <arch/uart.h>

void init(void) {
    uart_stdio_init(0, 9600);
}

int main(void) {
    init();

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
