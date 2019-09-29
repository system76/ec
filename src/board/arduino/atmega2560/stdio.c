#include <stdio.h>

#include "include/stdio.h"
#include "include/uart.h"

struct Uart * stdio_uart = NULL;

int stdio_get(FILE * stream) {
    return (int)uart_read(stdio_uart);
}

int stdio_put(char data, FILE * stream) {
    if (data == '\n') {
        uart_write(stdio_uart, '\r');
    }
    uart_write(stdio_uart, (unsigned char)data);
    return 0;
}

FILE stdio_file = FDEV_SETUP_STREAM(stdio_put, stdio_get, _FDEV_SETUP_RW);

void stdio_init(int num, unsigned long baud) {
    struct Uart * uart = uart_new(num);
    if(uart != NULL) {
        uart_init(uart, baud);
        stdio_uart = uart;
        stdin = stdout = stderr = &stdio_file;
    }
}
