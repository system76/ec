// SPDX-License-Identifier: GPL-3.0-only

#include <8051.h>
#include <stdint.h>

#include <board/dgpu.h>
#include <board/smfi.h>
#include <common/macro.h>
#include <ec/pwm.h>
#include <ec/scratch.h>

// Include scratch ROM
uint8_t __code __at(SCRATCH_OFFSET) scratch_rom[] = {
#include <scratch.h>
};

// Enter or exit scratch ROM
void scratch_trampoline(void) {
    // Set fans to 100%
    PWM_REG(CPU_FAN1) = 0xFF;
#ifdef CPU_FAN2
    PWM_REG(CPU_FAN2) = 0xFF;
#endif
#if HAVE_DGPU
    PWM_REG(GPU_FAN1) = 0xFF;
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
