#include <arch/reset.h>

void reset(void) {
    __asm__("ljmp 0");
}
