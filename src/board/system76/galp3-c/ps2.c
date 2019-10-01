#include <board/ps2.h>

void ps2_init(void) {
    *(PS2_1.control) = 0x11;
    *(PS2_1.interrupt) = 0x04;

    *(PS2_2.control) = 0x41;
    *(PS2_2.interrupt) = 0x04;

    *(PS2_3.control) = 0x41;
    *(PS2_3.interrupt) = 0x04;
}
