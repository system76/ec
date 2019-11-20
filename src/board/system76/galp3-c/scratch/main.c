#include <stdbool.h>
#include <stdint.h>

volatile uint8_t __xdata __at(0x103B) ECINDAR0;
volatile uint8_t __xdata __at(0x103C) ECINDAR1;
volatile uint8_t __xdata __at(0x103D) ECINDAR2;
volatile uint8_t __xdata __at(0x103E) ECINDAR3;
volatile uint8_t __xdata __at(0x103F) ECINDDR;

#define FLASH_SPI 0x00000000
#define FLASH_EMBEDDED 0x40000000

static int flash_transaction(uint32_t offset, uint8_t * data, int length, bool read) {
    int i;
    for (i = 0; i < length; i++, offset++) {
        ECINDAR3 = (uint8_t)(offset >> 24);
        ECINDAR2 = (uint8_t)(offset >> 16);
        ECINDAR1 = (uint8_t)(offset >> 8);
        ECINDAR0 = (uint8_t)(offset);
        if (read) {
            data[i] = ECINDDR;
        } else {
            ECINDDR = data[i];
        }
    }
    return i;
}

// Main program while running in scratch ROM
void main(void) {
    // TODO: implement flashing protocol and prevent access to flash
	// data (i.e. calling functions in CSEG) during this process
	for (;;) {}
}
