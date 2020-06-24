#include <8051.h>
#include <stdint.h>

#include <board/dgpu.h>
#include <board/smfi.h>
#include <common/macro.h>
#include <ec/pwm.h>

// Include scratch ROM
uint8_t __code __at(SCRATCH_OFFSET) scratch_rom[] = {
    #include <scratch.h>
};

#if __EC__ == it5570e
    // SCAR0 is stored in processor cache, not in xram
    volatile uint8_t __xdata __at(0x1040) SCAR0L;
    volatile uint8_t __xdata __at(0x1041) SCAR0M;
    volatile uint8_t __xdata __at(0x1042) SCAR0H;
    #define SCARL SCAR0L
    #define SCARM SCAR0M
    #define SCARH SCAR0H
#elif __EC__ == it8587e
    // SCAR1 is in xram at 0x800-0xC00
    volatile uint8_t __xdata __at(0x1043) SCAR1L;
    volatile uint8_t __xdata __at(0x1044) SCAR1M;
    volatile uint8_t __xdata __at(0x1045) SCAR1H;
    #define SCARL SCAR1L
    #define SCARM SCAR1M
    #define SCARH SCAR1H
#else
    #error "scratch.c unknown EC"
#endif

// Enter or exit scratch ROM
void scratch_trampoline(void) {
    // Set fans to 100%
    DCR2 = 0xFF;
#if HAVE_DGPU
    DCR4 = 0xFF;
#endif

    //TODO: Clear keyboard presses

    // Start watchdog timer
    smfi_watchdog();

    // Disable interrupts
    EA = 0;

    // Use DMA mapping to copy flash data
    SCARH = 0x80;
    SCARL = (uint8_t)(SCRATCH_OFFSET);
    SCARM = (uint8_t)(SCRATCH_OFFSET >> 8);
    SCARH = 0;

    // Jump to scratch reset function
    __asm__("ljmp " xstr(SCRATCH_OFFSET));
}
