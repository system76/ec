#include <8051.h>
#include <stdint.h>

#include <board/smfi.h>
#include <common/macro.h>

// Include scratch ROM
uint8_t __code __at(SCRATCH_OFFSET) scratch_rom[] = {
    #include <scratch.h>
};

// SCAR1 is in xram at 0x800-0xC00
volatile uint8_t __xdata __at(0x1043) SCAR1L;
volatile uint8_t __xdata __at(0x1044) SCAR1M;
volatile uint8_t __xdata __at(0x1045) SCAR1H;

// Enter or exit scratch ROM
void scratch_trampoline(void) {
    smfi_watchdog();

    // Disable interrupts
    EA = 0;

    // Use DMA mapping to copy flash data
    SCAR1H = 0x80;
    SCAR1L = (uint8_t)(SCRATCH_OFFSET);
    SCAR1M = (uint8_t)(SCRATCH_OFFSET >> 8);
    SCAR1H = 0;

    // Jump to scratch reset function
    __asm__("ljmp " xstr(SCRATCH_OFFSET));
}
