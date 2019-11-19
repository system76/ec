#include <board/peci.h>
#include <common/debug.h>
#include <ec/pwm.h>

int16_t peci_offset = 0;
int16_t peci_temp = 0;
uint8_t peci_duty = 0;

// Tjunction = 100C for i7-8565U (and probably the same for all WHL-U)
#define T_JUNCTION 10000

// PECI information can be found here: https://www.intel.com/content/dam/www/public/us/en/documents/design-guides/core-i7-lga-2011-guide.pdf
void peci_event(void) {
    // Wait for completion
    while (HOSTAR & 1) {}
    // Clear status
    HOSTAR = HOSTAR;

    // Enable PECI, clearing data fifo's
    HOCTLR = (1 << 5) | (1 << 3);
    // Set address to default
    HOTRADDR = 0x30;
    // Set write length
    HOWRLR = 1;
    // Set read length
    HORDLR = 2;
    // Set command
    HOCMDR = 1;
    // Start transaction
    HOCTLR |= 1;

    // Wait for completion
    while (HOSTAR & 1) {}

    if (HOSTAR & (1 << 1)) {
        // Use result if finished successfully
        uint8_t low = HORDDR;
        uint8_t high = HORDDR;
        peci_offset = ((int16_t)high << 8) | (int16_t)low;
        peci_temp = T_JUNCTION + peci_offset;

        // Set fan based on temp, adapted from
        // https://github.com/pop-os/system76-power/blob/master/src/fan.rs#L218
        if (peci_temp >= 9000) {
            // 90C = 100%
            peci_duty = 255;
        } else if (peci_temp >= 8000) {
            // 80C = 50%
            peci_duty = 128;
        } else if (peci_temp >= 7500) {
            // 75C = 45%
            peci_duty = 115;
        } else if (peci_temp >= 6500) {
            // 65C = 40%
            peci_duty = 102;
        } else if (peci_temp >= 5500) {
            // 55C = 35%
            peci_duty = 90;
        } else if (peci_temp >= 4500) {
            // 45C = 30%
            peci_duty = 77;
        } else {
            // < 45C = 0%
            peci_duty = 0;
        }
    } else {
        // Default to 50% if there is an error
        peci_offset = 0;
        peci_temp = 0;
        peci_duty = 128;
    }

    if (peci_duty != DCR2) {
        DCR2 = peci_duty;
        DEBUG("PECI offset=%d, temp=%d = %d\n", peci_offset, peci_temp, peci_duty);
    }
}
