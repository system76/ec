#include <board/pwm.h>

void pwm_init(void) {
    // Set T0CHSEL to TACH0A and T1CHSEL to TACH1A
    TSWCTLR = 0;

    // Disable PWM
    ZTIER = 0;

    // Set prescaler to 1159 + 1
    C6CPRS = 0x87;
    C6MCPRS = 0x04;

    // Set cycle time to 255 + 1
    CTR2 = 0xFF;

    // Turn on the CPU fan at half blast (temperature control TODO)
    DCR2 = 0x80;

    // Set gating clock source
    PCSGR = 0xFF;

    // Set PWM2 output to use C6CPRS / CTR2
    PCSSGL = (0b10 << 4);

    // Enable PWM
    ZTIER = (1 << 1);
}
