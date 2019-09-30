#include <board/kbscan.h>

void kbscan_init(void) {
    KSOCTRL = 0x05;
    KSICTRLR = 0x04;

    // Set all outputs to GPIO mode and high
    KSOH2 = 0xFF;
    KSOH1 = 0xFF;
    KSOL = 0xFF;
    KSOHGCTRL = 0xFF;
    KSOHGOEN = 0xFF;
    KSOLGCTRL = 0xFF;
    KSOLGOEN = 0xFF;
}
