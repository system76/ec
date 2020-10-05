#include <board/fan.h>

bool fan_max = false;

void fan_reset(void) {
    // Do not manually set fans to maximum speed
    fan_max = false;
}
