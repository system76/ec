#include <board/pwm.h>

void pwm_init(void) {
    // Set T0CHSEL to TACH0A and T1CHSEL to TACH1A
    TSWCTLR = 0;

    // Disable PWM
    ZTIER = 0;

    // Set prescalar clock frequency to EC clock
    PCFSR = 0b01;

    // Set clock prescaler to 0 + 1
    C0CPRS = 0;

    // Set cycle time to 255 + 1
    CTR0 = 255;

    // Turn on the CPU fan at half blast (temperature control TODO)
    DCR2 = 128;

    // Enable PWM
    ZTIER = (1 << 1);
}
