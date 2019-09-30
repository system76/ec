#include <board/pwm.h>

void pwm_init(void) {
    // Set T0CHSEL to TACH0A and T1CHSEL to TACH1A
    TSWCTLR = 0;
    // Turn on the CPU fan at full blast (temperature control TODO)
    DCR2 = 0xFF;
}
