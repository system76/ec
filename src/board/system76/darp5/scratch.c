#include <stdbool.h>
#include <stdint.h>

#include <common/macro.h>

// Include scratch ROM, must be less than 4096 bytes, controlled by makefile
uint8_t __code scratch_rom[] = {
    #include <scratch.h>
};

// Scratch RAM 0, 1, 2, 3, and 4 are 4096 bytes total, located at 0x0000 bytes
volatile uint8_t __xdata __at(0x0000) scratch_ram[4096];

volatile uint8_t __xdata __at(0x1040) SCAR0L;
volatile uint8_t __xdata __at(0x1041) SCAR0M;
volatile uint8_t __xdata __at(0x1042) SCAR0H;

volatile uint8_t __xdata __at(0x1043) SCAR1L;
volatile uint8_t __xdata __at(0x1044) SCAR1M;
volatile uint8_t __xdata __at(0x1045) SCAR1H;

volatile uint8_t __xdata __at(0x1046) SCAR2L;
volatile uint8_t __xdata __at(0x1047) SCAR2M;
volatile uint8_t __xdata __at(0x1048) SCAR2H;

volatile uint8_t __xdata __at(0x1049) SCAR3L;
volatile uint8_t __xdata __at(0x104A) SCAR3M;
volatile uint8_t __xdata __at(0x104B) SCAR3H;

volatile uint8_t __xdata __at(0x104C) SCAR4L;
volatile uint8_t __xdata __at(0x104D) SCAR4M;
volatile uint8_t __xdata __at(0x104E) SCAR4H;

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
    // Uses SCAR0, 1, 2, 3, and 4 which are mapped at 0x0000 in data space and are
    // 4096 bytes in size.
    
    // Disable interrupts
    EA = 0;

    // Ensure pdata is not used!
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
    // Set scratch RAM 1 mapping at 0x0800 and enable
    SCAR1L = 0x00;
    SCAR1M = 0x08;
    SCAR1H = 0x00;
    // Set scratch RAM 2 mapping at 0x0C00 and enable
    SCAR2L = 0x00;
    SCAR2M = 0x0C;
    SCAR2H = 0x00;
    // Set scratch RAM 3 mapping at 0x0E00 and enable
    SCAR3L = 0x00;
    SCAR3M = 0x0E;
    SCAR3H = 0x00;
    // Set scratch RAM 4 mapping at 0x0F00 and enable
    SCAR4L = 0x00;
    SCAR4M = 0x0F;
    SCAR4H = 0x00;

    // Jump to reset function
    __asm__("ljmp 0");
}

// Finish segment located at 0x1000
static void scratch_end(void) __naked {
	__asm
		.area CSEG (REL,CODE)
	__endasm;
}
