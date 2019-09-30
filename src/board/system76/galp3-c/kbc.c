#include <board/kbc.h>

void kbc_init(void) {
    *(KBC.irq) = 0;
    *(KBC.control) = 0x48;
}
