#include <board/dac.h>

void dac_init(void) {
    // Enable DAC2, used for KBLIGHT_ADJ
    DACPDREG &= ~(1 << 2);
    // Set DAC2 to 0V
    DACDAT2 = 0;
}
