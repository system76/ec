#ifndef _ARCH_UART_H
#define _ARCH_UART_H

#include <stdint.h>

struct Uart {
    volatile uint8_t * a;
    volatile uint8_t * b;
    volatile uint8_t * c;
    volatile uint8_t * data;
    volatile uint8_t * baud_l;
    volatile uint8_t * baud_h;
    uint8_t a_read;
    uint8_t a_write;
    uint8_t a_init;
    uint8_t b_init;
    uint8_t c_init;
};

void uart_init(struct Uart * uart, unsigned long baud);

int uart_count();
struct Uart * uart_new(int num);

unsigned char uart_can_read(struct Uart * uart);
unsigned char uart_can_write(struct Uart * uart);

unsigned char uart_read(struct Uart * uart);
void uart_write(struct Uart * uart, unsigned char data);

extern struct Uart * uart_stdio;
void uart_stdio_init(int num, unsigned long baud);

#endif // _ARCH_UART_H
