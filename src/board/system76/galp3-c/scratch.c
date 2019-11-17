#include <stdbool.h>
#include <stdint.h>

volatile uint8_t __xdata __at(0x103B) ECINDAR0;
volatile uint8_t __xdata __at(0x103C) ECINDAR1;
volatile uint8_t __xdata __at(0x103D) ECINDAR2;
volatile uint8_t __xdata __at(0x103E) ECINDAR3;
volatile uint8_t __xdata __at(0x103F) ECINDDR;
volatile uint8_t __xdata __at(0x1040) SCAR0L;
volatile uint8_t __xdata __at(0x1041) SCAR0M;
volatile uint8_t __xdata __at(0x1042) SCAR0H;
volatile uint8_t __xdata __at(0x1043) SCAR1L;
volatile uint8_t __xdata __at(0x1044) SCAR1M;
volatile uint8_t __xdata __at(0x1045) SCAR1H;

// Create new segment located at 0x1000
static void scratch_start(void) __naked
{
	__asm
		.area SCRATCH (ABS,CODE)
	__endasm;
	__asm
		.org 0x1000
	__endasm;
}

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
static void scratch_main(void) {
    // TODO: implement flashing protocol and prevent access to flash
	// data (i.e. calling functions in CSEG) during this process
	for (;;) {}
}

// Finish segment located at 0x1000
static void scratch_end(void) __naked
{
	__asm
		.area CSEG (REL,CODE)
	__endasm;
}

// Enter scratch ROM
void scratch_trampoline(void) {
	// Uses SCAR1 which is mapped at 0x0800 in data space and is 1024 bytes in
	// size. SCAR0 cannot be used due to __pdata overwriting it.

    // Set flag to use DMA on next write
    SCAR1H = 0x80;
    // Set mapping at 0x1000
    SCAR1L = 0x00;
    SCAR1M = 0x10;
    // Set DMA and enable code space mapping
    SCAR1H = 0x00;

    scratch_main();
}
