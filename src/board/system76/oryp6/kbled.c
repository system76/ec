#include <board/kbled.h>
#include <ec/pwm.h>

void kbled_init(void) {
    //TODO: enable PWMs
    kbled_reset();
}

void kbled_reset(void) {
    // Set brightness and color
    kbled_set_color(0xFFFFFF);
    kbled_set(0x00);
}

uint8_t kbled_get(void) {
    // Get PWM for power
    return DCR0;
}

void kbled_set(uint8_t level) {
    // Set PWM for power
    DCR0 = level;
}

void kbled_set_color(uint32_t color) {
    // Set PWM for blue component
    DCR7 = (uint8_t)(color);

    // Set PWM for green component
    DCR6 = (uint8_t)(color >> 8);

    // Set PWM for red component
    DCR5 = (uint8_t)(color >> 16);
}
