#include <8051.h>
#include <stdint.h>

#include <common/macro.h>

// Include scratch ROM
uint8_t __code __at(SCRATCH_OFFSET) scratch_rom[] = {
    #include <scratch.h>
};

// SCAR0 is stored in processor cache, not in xram
volatile uint8_t __xdata __at(0x1040) SCAR0L;
volatile uint8_t __xdata __at(0x1041) SCAR0M;
volatile uint8_t __xdata __at(0x1042) SCAR0H;

// Enter or exit scratch ROM
void scratch_trampoline(void) {
    // Disable interrupts
    EA = 0;

    // Use DMA mapping to copy flash data
    SCAR0H = 0x80;
    SCAR0L = (uint8_t)(SCRATCH_OFFSET);
    SCAR0M = (uint8_t)(SCRATCH_OFFSET >> 8);
    SCAR0H = 0;

    // Jump to scratch reset function
    __asm__("ljmp " xstr(SCRATCH_OFFSET));
}
