// SPDX-License-Identifier: GPL-3.0-only

#include <stdio.h>
#include <avr/io.h>

#include <arch/uart.h>
#include <board/cpu.h>

#define UART(N) \
    { \
        &UCSR ## N ## A, \
        &UCSR ## N ## B, \
        &UCSR ## N ## C, \
        &UDR ## N, \
        &UBRR ## N ## L, \
        &UBRR ## N ## H, \
        _BV(RXC ## N), \
        _BV(UDRE ## N), \
        0, \
        _BV(RXEN ## N) | _BV(TXEN ## N), \
        _BV(UCSZ ## N ## 1) | _BV(UCSZ ## N ## 0) \
    }

#if defined(__AVR_ATmega328P__)
    static struct Uart UARTS[] = {
        UART(0)
    };
#elif defined(__AVR_ATmega32U4__)
    static struct Uart UARTS[] = {
        UART(1)
    };
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    static struct Uart UARTS[] = {
        UART(0),
        UART(1),
        UART(2),
        UART(3)
    };
#else
    #error "Could not find UART definitions"
#endif

int uart_count() {
    return sizeof(UARTS)/sizeof(struct Uart);
}

struct Uart * uart_new(int num) {
    if (num < uart_count()) {
        return &UARTS[num];
    } else {
        return NULL;
    }
}

void uart_init(struct Uart * uart, unsigned long baud) {
    unsigned long baud_prescale = (F_CPU / (baud * 16UL)) - 1;
    *(uart->baud_h) = (uint8_t)(baud_prescale>>8);
    *(uart->baud_l) = (uint8_t)(baud_prescale);
    *(uart->a) = uart->a_init;
    *(uart->b) = uart->b_init;
    *(uart->c) = uart->c_init;
}

unsigned char uart_can_read(struct Uart * uart) {
    return (*(uart->a)) & uart->a_read;
}

unsigned char uart_read(struct Uart * uart) {
    while (!uart_can_read(uart)) ;
    return *(uart->data);
}

unsigned char uart_can_write(struct Uart * uart) {
    return (*(uart->a)) & uart->a_write;
}

void uart_write(struct Uart * uart, unsigned char data) {
    while (!uart_can_write(uart)) ;
    *(uart->data) = data;
}

struct Uart * uart_stdio = NULL;

int uart_stdio_get(FILE * stream) {
    return (int)uart_read(uart_stdio);
}

int uart_stdio_put(char data, FILE * stream) {
    uart_write(uart_stdio, (unsigned char)data);
    return 0;
}

FILE uart_stdio_file = FDEV_SETUP_STREAM(uart_stdio_put, uart_stdio_get, _FDEV_SETUP_RW);

void uart_stdio_init(int num, unsigned long baud) {
    struct Uart * uart = uart_new(num);
    if(uart != NULL) {
        uart_init(uart, baud);
        uart_stdio = uart;
        stdin = stdout = stderr = &uart_stdio_file;
    }
}
