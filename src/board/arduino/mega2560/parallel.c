// High resolution pinout can be found here:
// https://osoyoo.com/wp-content/uploads/2017/08/arduino_mega_2560_pinout.png

#include <stdint.h>
#include <stdio.h>
#include <board/cpu.h>
#include <util/delay.h>

#include <arch/gpio.h>
#include <arch/uart.h>

// Mapping of 24-pin ribbon cable to parallel pins. See schematic
#define PINS \
    /* Data (KSO0 - KSO7) - bi-directional */ \
    PIN(d0, 1) \
    PIN(d1, 2) \
    PIN(d2, 3) \
    PIN(d3, 7) \
    PIN(d4, 9) \
    PIN(d5, 10) \
    PIN(d6, 13) \
    PIN(d7, 16) \
    /* Wait# (KSO9) - input */ \
    /*  low to indicate cycle may begin, high to indicate cycle may end */ \
    PIN(wait_n, 18) \
    /* Write# (KSI0) - output */ \
    /*  low to indicate write cycle, high to indicate read cycle */ \
    PIN(write_n, 4) \
    /* DataStrobe# (KSI1) - output */ \
    /*  low indicates a data cycle */ \
    PIN(data_n, 5) \
    /* Reset# (KSI2) - output */ \
    /*  low requests device reset */ \
    PIN(reset_n, 6) \
    /* AddressStrobe# (KSI3) - output */ \
    /*  low indicates an address cycle */ \
    PIN(addr_n, 8) \
    /* Strap0 (KSI4) */ \
    /*  1K-Ohm pull-down resistor */ \
    PIN(strap_0, 11) \
    /* Strap1 (KSI5) */ \
    /*  1K-Ohm pull-down resistor */ \
    PIN(strap_1, 12)

#define DATA_BITS \
    DATA_BIT(0) \
    DATA_BIT(1) \
    DATA_BIT(2) \
    DATA_BIT(3) \
    DATA_BIT(4) \
    DATA_BIT(5) \
    DATA_BIT(6) \
    DATA_BIT(7)

// Mapping of 24-pin ribbon cable to GPIOs
static struct Gpio GPIOS[24] = {
    GPIO(L, 5), GPIO(L, 4),
    GPIO(L, 7), GPIO(L, 6),
    GPIO(G, 1), GPIO(G, 0),
    GPIO(D, 7), GPIO(G, 2),
    GPIO(C, 1), GPIO(C, 0),
    GPIO(C, 3), GPIO(C, 2),
    GPIO(C, 5), GPIO(C, 4),
    GPIO(C, 7), GPIO(C, 6),
    GPIO(A, 6), GPIO(A, 7),
    GPIO(A, 4), GPIO(A, 5),
    GPIO(A, 2), GPIO(A, 3),
    GPIO(A, 0), GPIO(A, 1),
};

// Parallel struct definition
// See http://efplus.com/techref/io/parallel/1284/eppmode.htm
struct Parallel {
    #define PIN(N, P) struct Gpio * N;
    PINS
    #undef PIN
};

// Parallel struct instance
static struct Parallel PORT = {
    #define PIN(N, P) .N = &GPIOS[P - 1],
    PINS
    #undef PIN
};

// Set port to all high-impedance inputs
void parallel_hiz(struct Parallel * port) {
    #define PIN(N, P) \
        gpio_set_dir(port->N, false); \
        gpio_set(port->N, false);
    PINS
    #undef PIN
}

// Set port to initial state required before being able to perform cycles
void parallel_reset(struct Parallel * port) {
    parallel_hiz(port);

    // Set reset line low
    gpio_set_dir(port->reset_n, true);

    // Wait 1 microsecond
    _delay_us(1);
    
    // Make sure strobes are high outputs
    gpio_set(port->data_n, true);
    gpio_set(port->addr_n, true);
    gpio_set_dir(port->data_n, true);
    gpio_set_dir(port->addr_n, true);

    // Set write line high output
    gpio_set(port->write_n, true);
    gpio_set_dir(port->write_n, true);

    // Pull up wait line
    gpio_set(port->wait_n, true);

    // Pull up data lines
    #define DATA_BIT(B) gpio_set(port->d ## B, true);
    DATA_BITS
    #undef DATA_BIT

    //TODO: something with straps
    
    // Wait 1 microsecond
    _delay_us(1);

    // Set reset line high, ending reset
    gpio_set(port->reset_n, true);
}

//TODO: timeout
int parallel_transaction(struct Parallel * port, uint8_t * data, int length, bool read, bool addr) {
    int i;
    for (i = 0; i < length; i++) {
        // Wait for wait line to be low
        while (gpio_get(port->wait_n)) {}

        // Set write line
        gpio_set(port->write_n, read);
        
        if (!read) {
            // Set data
            uint8_t byte = data[i];
            #define DATA_BIT(B) \
                gpio_set(port->d ## B, (byte & (1 << B)) > 0); \
                gpio_set_dir(port->d ## B, true);
            DATA_BITS
            #undef DATA_BIT
        }

        // Wait 1 microsecond
        _delay_us(1);

        if (addr) {
            // Set address strobe low
            gpio_set(port->addr_n, false);
        } else {
            // Set data strobe low
            gpio_set(port->data_n, false);
        }

        // Wait for wait line to be high
        while (!gpio_get(port->wait_n)) {}

        if (read) {
            uint8_t byte = 0;
            #define DATA_BIT(B) \
                if (gpio_get(port->d ## B)) byte |= (1 << B);
            DATA_BITS
            #undef DATA_BIT
            data[i] = byte;
        }

        if (addr) {
            // Set address strobe high
            gpio_set(port->addr_n, true);
        } else {
            // Set data strobe high
            gpio_set(port->data_n, true);
        }

        // Wait 1 microsecond
        _delay_us(1);

        // Reset data lines to pull-up inputs
        #define DATA_BIT(B) \
            gpio_set_dir(port->d ## B, false); \
            gpio_set(port->d ## B, true);
        DATA_BITS
        #undef DATA_BIT

        // Set write line high
        gpio_set(port->write_n, true);
    }

    return i;
}

#define parallel_get_address(P, D, L) parallel_transaction(P, D, L, true, true)
#define parallel_set_address(P, D, L) parallel_transaction(P, D, L, false, true)
#define parallel_read(P, D, L) parallel_transaction(P, D, L, true, false)
#define parallel_write(P, D, L) parallel_transaction(P, D, L, false, false)

int serial_transaction(uint8_t * data, int length, bool read) {
    int i;
    for (i = 0; i < length; i++) {
        if (read) {
            data[i] = (uint8_t)uart_read(uart_stdio);
        } else {
            uart_write(uart_stdio, (unsigned char)data[i]);
        }
    }

    return i;
}

#define serial_read(D, L) serial_transaction(D, L, true)
#define serial_write(D, L) serial_transaction(D, L, false)

int parallel_main(void) {
    int res = 0;

    struct Parallel * port = &PORT;
    parallel_reset(port);

    static uint8_t data[256];
    char command;
    int length;
    for (;;) {
        // Read command and length
        res = serial_read(data, 2);
        if (res < 0) goto err;
        // Command is a character
        command = (char)data[0];
        // Length is received data + 1
        length = (int)data[1] + 1;
        // Truncate length to size of data
        if (length > sizeof(data)) length = sizeof(data);

        switch (command) {
            // Echo
            case 'E':
                // Read data from serial
                res = serial_read(data, length);
                if (res < 0) goto err;

                // Write data to serial
                res = serial_write(data, length);
                if (res < 0) goto err;

                break;

            // Set address
            case 'A':
                // Read data from serial
                res = serial_read(data, length);
                if (res < 0) goto err;

                // Write address to parallel
                res = parallel_set_address(port, data, length);
                if (res < 0) goto err;

                break;

            // Read data
            case 'R':
                // Read data from parallel
                res = parallel_read(port, data, length);
                if (res < 0) goto err;
                for (res = 0; res < length; res++) {
                    data[res] = (uint8_t)res;
                }

                // Write data to serial
                res = serial_write(data, length);
                if (res < 0) goto err;

                break;

            // Write data
            case 'W':
                // Read data from serial
                res = serial_read(data, length);
                if (res < 0) goto err;

                // Write data to parallel
                res = parallel_write(port, data, length);
                if (res < 0) goto err;

                break;
        }

        // Send ACK
        data[0] = '\r';
        res = serial_write(data, 1);
        if (res < 0) goto err;
    }

err:
    parallel_hiz(port);

    return res;
}
