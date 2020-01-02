// High resolution pinout can be found here:
// https://osoyoo.com/wp-content/uploads/2017/08/arduino_mega_2560_pinout.png

#include <stdint.h>
#include <stdio.h>
#include <board/cpu.h>
#include <util/delay.h>

#include <arch/gpio.h>

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

int parallel_read_at(struct Parallel * port, uint8_t address, uint8_t * data, int length) {
    int res;

    res = parallel_set_address(port, &address, 1);
    if (res < 0) return res;

    return parallel_read(port, data, length);
}

int parallel_write_at(struct Parallel * port, uint8_t address, uint8_t * data, int length) {
    int res;

    res = parallel_set_address(port, &address, 1);
    if (res < 0) return res;

    return parallel_write(port, data, length);
}

int parallel_flash_transaction(struct Parallel * port, uint32_t address, uint8_t * data, int length, bool read) {
    int res;
    uint8_t byte;

    // ECINDAR3
    byte = (uint8_t)(address >> 24);
    res = parallel_write_at(port, 7, &byte, 1);
    if (res < 0) return res;

    // ECINDAR2
    byte = (uint8_t)(address >> 16);
    res = parallel_write_at(port, 6, &byte, 1);
    if (res < 0) return res;
    
    // ECINDAR1
    byte = (uint8_t)(address >> 8);
    res = parallel_write_at(port, 5, &byte, 1);
    if (res < 0) return res;

    // ECINDAR0
    byte = (uint8_t)(address);
    res = parallel_write_at(port, 4, &byte, 1);
    if (res < 0) return res;

    // ECINDDR
    if (read) {
        return parallel_read_at(port, 8, data, length);
    } else {
        return parallel_write_at(port, 8, data, length);
    }
}

void parallel_host(void) {
    printf("Parallel host. Reset to exit\n");

    struct Parallel * port = &PORT;
    parallel_reset(port);

    for (;;) {
        int c = getchar();
        if (c < 0) break;

        if (c == '\r') {
            printf("Reading chip ID\n");

            uint8_t id[3];

            parallel_read_at(port, 0, &id[0], 1);
            parallel_read_at(port, 1, &id[1], 1);
            parallel_read_at(port, 2, &id[2], 1);

            printf("  ID %02X%02X version %d\n", id[0], id[1], id[2]);

            uint8_t byte;

            byte = 0;
            parallel_flash_transaction(port, 0x7FFFFE00, &byte, 1, false);

            uint8_t command[5] = {0xB, 0, 0, 0, 0};
            parallel_flash_transaction(port, 0x7FFFFD00, command, 5, false);

            int i;
            for (i = 0; i < 256; i++) {
                parallel_flash_transaction(port, 0x7FFFFD00, &byte, 1, true);
                printf("%02X: %02X\n", i, byte);
            }

            byte = 0;
            parallel_flash_transaction(port, 0x7FFFFE00, &byte, 1, false);
        }
    }

    parallel_hiz(port);
}

void parallel_peripheral(void) {
    printf("Parallel peripheral (WIP, writes only). Reset to exit\n");

    struct Parallel * port = &PORT;
    parallel_hiz(port);

    // Strobes are high when inactive
    bool last_data_n = true;
    bool last_addr_n = true;

    for (;;) {
        // Pull wait line low
        gpio_set_dir(port->wait_n, true);

        // Read data strobe and edge detect
        bool data_n = gpio_get(port->data_n);
        bool data_edge = last_data_n && !data_n;

        // Read address strobe and edge detect
        bool addr_n = gpio_get(port->addr_n);
        bool addr_edge = last_addr_n && !addr_n;

        // If not in reset
        if (gpio_get(port->reset_n)) {
            // On the falling edge of either strobe
            if (data_edge || addr_edge) {
                // Check if read or write cycle
                bool read = gpio_get(port->write_n);
        
                // Read data
                uint8_t byte = 0;
                #define DATA_BIT(B) \
                    if (gpio_get(port->d ## B)) byte |= (1 << B);
                DATA_BITS
                #undef DATA_BIT

                //TODO: Check if strobe fell while reading

                // Release wait line
                gpio_set_dir(port->wait_n, false);

                if (data_edge) {
                    putchar('d');
                }

                if (addr_edge) {
                    putchar('a');
                }

                if (read) {
                    putchar('<');
                } else {
                    putchar('>');
                }

                printf("%02X\n", byte);

                // Wait 1 microsecond
                _delay_us(1);

            }
        }

        last_data_n = data_n;
        last_addr_n = addr_n;
    }
}

void parallel_spy(void) {
    printf("Parallel spy. Reset to exit\n");

    struct Parallel * port = &PORT;
    parallel_hiz(port);

    // Strobes are high when inactive
    bool last_data_n = true;
    bool last_addr_n = true;

    for (;;) {
        // Read data strobe and edge detect
        bool data_n = gpio_get(port->data_n);
        bool data_edge = last_data_n && !data_n;

        // Read address strobe and edge detect
        bool addr_n = gpio_get(port->addr_n);
        bool addr_edge = last_addr_n && !addr_n;

        // If not in reset
        if (gpio_get(port->reset_n)) {
            // On the falling edge of either strobe
            if (data_edge || addr_edge) {
                // Check if read or write cycle
                bool read = gpio_get(port->write_n);
        
                // Read data
                uint8_t byte = 0;
                #define DATA_BIT(B) \
                    if (gpio_get(port->d ## B)) byte |= (1 << B);
                DATA_BITS
                #undef DATA_BIT

                //TODO: Check if strobe fell while reading

                if (data_edge) {
                    putchar('d');
                }

                if (addr_edge) {
                    putchar('a');
                }

                if (read) {
                    putchar('<');
                } else {
                    putchar('>');
                }

                printf("%02X\n", byte);
            }
        }

        last_data_n = data_n;
        last_addr_n = addr_n;
    }
}

int parallel_main(void) {
    int res = 0;

    struct Parallel * port = &PORT;
    parallel_reset(port);

    uint8_t b;
    int c;
    for (;;) {
        c = getchar();
        if (c < 0) goto err;

        switch (c) {
            // Echo
            case 'E':
                break;

            // Set address
            case 'A':
                c = getchar();
                if (c < 0) goto err;

                b = (uint8_t)c;
                res = parallel_set_address(port, &b, 1);
                if (res < 0) goto err;

                break;

            // Read data
            case 'R':
                res = parallel_read(port, &b, 1);
                if (res < 0) goto err;

                c = putchar((int)b);
                if (c < 0) goto err;

                break;

            // Write data
            case 'W':
                c = getchar();
                if (c < 0) goto err;

                b = (uint8_t)c;
                res = parallel_write(port, &b, 1);
                if (res < 0) goto err;

                break;
        }

        c = putchar('\r');
        if (c < 0) goto err;
    }

err:
    parallel_hiz(port);

    return res;
}
