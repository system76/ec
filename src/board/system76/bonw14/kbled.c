#include <board/kbled.h>
#include <ec/pwm.h>

void kbled_init(void) {
    kbled_reset();
}

void kbled_reset(void) {
    // Set brightness and color
    kbled_set_color(0xFFFFFF);
    kbled_set(0x00);
}

uint8_t kbled_get(void) {
    return 0;
}

void kbled_set(uint8_t level) {
    //TODO - I2C interface?
    level = level;
}

void kbled_set_color(uint32_t color) {
    //TODO - I2C interface?
    color = color;
}
