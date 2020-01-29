#include <stdbool.h>
#include <stdint.h>

#include <common/macro.h>

// Include scratch ROM, must be less than 4096 bytes, controlled by makefile
uint8_t __code scratch_rom[] = {
    #include <scratch.h>
};

// Scratch RAM 0 is 4096 bytes, located at 0x0000 bytes
volatile uint8_t __xdata __at(0x0000) scratch_ram[4096];

volatile uint8_t __xdata __at(0x1040) SCAR0L;
volatile uint8_t __xdata __at(0x1041) SCAR0M;
volatile uint8_t __xdata __at(0x1042) SCAR0H;

// Create new segment located at 0x1000, after scratch ROM mapping
static void scratch_start(void) __naked {
	__asm
		.area SCRATCH (ABS,CODE)
	__endasm;
	__asm
		.org 0x1000
	__endasm;
}

// Enter or exit scratch ROM
void scratch_trampoline(void) {
	// Uses SCAR0 which is mapped at 0x0000 in data space and are
    // 4096 bytes in size.
    
    //TODO: ensure pdata is not used!

    if (SCAR0H == 0x00) {
        // Disable scratch RAM mapping
        SCAR0H = 0b11;
    } else {
        int __data i;
        // Copy scratch ROM
        for (i = 0; i < ARRAY_SIZE(scratch_rom) && i < ARRAY_SIZE(scratch_ram); i++) {
            scratch_ram[i] = scratch_rom[i];
        }

        // Fill the rest with nop
        for (; i < ARRAY_SIZE(scratch_ram); i++) {
            scratch_ram[i] = 0x00;
        }

        // Set scratch RAM 0 mapping at 0x0000 and enable
        SCAR0L = 0x00;
        SCAR0M = 0x00;
        SCAR0H = 0x00;
    }

    // Jump to reset function
    __asm__("ljmp 0");
}

// Finish segment located at 0x1000
static void scratch_end(void) __naked {
	__asm
		.area CSEG (REL,CODE)
	__endasm;
}
