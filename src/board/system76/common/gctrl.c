#include <board/gctrl.h>

void gctrl_init(void) {
    SPCTRL1 = 0xCB;
    BADRSEL = 0;
}
