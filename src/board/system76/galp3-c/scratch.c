#include <stdbool.h>
#include <stdint.h>

#include <common/macro.h>

// Include scratch ROM, must be less than 1024 bytes, controlled by makefile
uint8_t __code scratch_rom[] = {
    #include <scratch.h>
};

// Scratch RAM 1 is 1024 bytes, located at 2048 bytes
volatile uint8_t __xdata __at(0x0800) scratch_ram_1[1024];

volatile uint8_t __xdata __at(0x1043) SCAR1L;
volatile uint8_t __xdata __at(0x1044) SCAR1M;
volatile uint8_t __xdata __at(0x1045) SCAR1H;

// Create new segment located at 0x1000, after scratch ROM mapping
static void scratch_start(void) __naked {
	__asm
		.area SCRATCH (ABS,CODE)
	__endasm;
	__asm
		.org 0x1000
	__endasm;
}

// Enter scratch ROM
int scratch_trampoline(void) {
	// Uses SCAR1 which is mapped at 0x0800 in data space and is 1024 bytes in
	// size. SCAR0 cannot be used due to __pdata overwriting it.

    int i;
    // Copy scratch ROM
    for (i = 0; i < ARRAY_SIZE(scratch_rom) && i < ARRAY_SIZE(scratch_ram_1); i++) {
        scratch_ram_1[i] = scratch_rom[i];
    }

    // Fill the rest with nop
    for (; i < ARRAY_SIZE(scratch_ram_1); i++) {
        scratch_ram_1[i] = 0x00;
    }

    // Set mapping at 0x0000 and enable
    SCAR1L = 0x00;
    SCAR1M = 0x00;
    SCAR1H = 0x00;

    // Jump to scratch reset function
    __asm__("ljmp 0");

    // Should never happen
    return 0;
}

// Finish segment located at 0x1000
static void scratch_end(void) __naked {
	__asm
		.area CSEG (REL,CODE)
	__endasm;
}
