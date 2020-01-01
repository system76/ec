// High resolution pinout can be found here:
// https://osoyoo.com/wp-content/uploads/2017/08/arduino_mega_2560_pinout.png

#include <stdint.h>
#include <stdio.h>

#include <arch/gpio.h>

// Mapping of 24-pin ribbon cable to GPIOs
static struct Gpio gpios[24] = {
    GPIO(B, 1), GPIO(B, 0),
    GPIO(B, 3), GPIO(B, 2),
    GPIO(L, 1), GPIO(L, 0),
    GPIO(L, 3), GPIO(L, 2),
    GPIO(L, 5), GPIO(L, 4),
    GPIO(L, 7), GPIO(L, 6),
    GPIO(G, 1), GPIO(G, 0),
    GPIO(D, 7), GPIO(G, 2),
    GPIO(C, 1), GPIO(C, 0),
    GPIO(C, 3), GPIO(C, 2),
    GPIO(C, 5), GPIO(C, 4),
    GPIO(C, 7), GPIO(C, 6)
};

// See http://efplus.com/techref/io/parallel/1284/eppmode.htm
struct ParallelPins {
    // Data (KSO0 - KSO7) - bi-directional
    uint8_t d0;
    uint8_t d1;
    uint8_t d2;
    uint8_t d3;
    uint8_t d4;
    uint8_t d5;
    uint8_t d6;
    uint8_t d7;
    // Wait# (KSO9) - input
    //  low to indicate cycle may begin, high to indicate cycle may end
    uint8_t wait_n;
    // Write# (KSI0) - output
    //  low to indicate write cycle, high to indicate read cycle
    uint8_t write_n;
    // DataStrobe# (KSI1) - output
    //  low indicates a data cycle
    uint8_t data_n;
    // Reset# (KSI2) - output
    //  low requests device reset
    uint8_t reset_n;
    // AddressStrobe# (KSI3) - output
    //  low indicates an address cycle
    uint8_t addr_n;
    // Strap0 (KSI4)
    //  1K-Ohm pull-down resistor
    uint8_t strap_0;
    // Strap1 (KSI5)
    //  1K-Ohm pull-down resistor
    uint8_t strap_1;
};

// Mapping of 24-pin ribbon cable to parallel pins. See schematic
static struct ParallelPins pins = {
    .d0 = 1,
    .d1 = 2,
    .d2 = 3,
    .d3 = 7,
    .d4 = 9,
    .d5 = 10,
    .d6 = 13,
    .d7 = 16,
    .wait_n = 18,
    .write_n = 4,
    .data_n = 5,
    .reset_n = 6,
    .addr_n = 8,
    .strap_0 = 11,
    .strap_1 = 12,
};

#define PIN_GPIO(N) \
    struct Gpio * N = NULL; \
    if (pins.N > 0 && pins.N < 24) N = &gpios[pins.N - 1]; \
    if (!N) return; \
    gpio_set_dir(N, false); \
    gpio_set(N, false);

void parallel(void) {
    PIN_GPIO(d0);
    PIN_GPIO(d1);
    PIN_GPIO(d2);
    PIN_GPIO(d3);
    PIN_GPIO(d4);
    PIN_GPIO(d5);
    PIN_GPIO(d6);
    PIN_GPIO(d7);
    PIN_GPIO(wait_n);
    PIN_GPIO(write_n);
    PIN_GPIO(data_n);
    PIN_GPIO(reset_n);
    PIN_GPIO(addr_n);
    PIN_GPIO(strap_0);
    PIN_GPIO(strap_1);
}
