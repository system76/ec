#include <board/gctrl.h>

void gctrl_init(void) {
    SPCTRL1 = 0x03;
#ifdef POST_DEBUG
    SPCTRL1 |= 0xC8;
#endif
    BADRSEL = 0;
}
