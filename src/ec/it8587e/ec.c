#include <ec/ec.h>
#include <ec/gctrl.h>

void ec_init(void) {
    RSTS = 0x84;
}
