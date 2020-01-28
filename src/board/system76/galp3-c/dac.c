#include <board/dac.h>

void dac_init(void) {
    // Enable DAC5, used for KBLIGHT_ADJ
    DACPDREG &= ~(1 << 5);
    // Set DAC5 to 0V
    DACDAT5 = 0;
}
