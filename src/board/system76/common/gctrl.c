#include <board/gctrl.h>

void gctrl_init(void) {
    SPCTRL1 = 0x03;
    BADRSEL = 0;
}
