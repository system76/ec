#include <arch/delay.h>
#include <board/gpio.h>
#include <board/lid.h>
#include <board/pmc.h>
#include <common/debug.h>

bool lid_state = true;
bool lid_wake = false;

void lid_event(void) {
    static bool send_sci = true;

    // Check if the lid switch has changed
    bool new = gpio_get(&LID_SW_N);
    if (new != lid_state) {
        DEBUG("Lid ");
        if (new) {
            DEBUG("open\n");

            if (lid_wake) {
                gpio_set(&SWI_N, false);

                //TODO: find correct delay
                delay_ticks(10);

                gpio_set(&SWI_N, true);

                lid_wake = false;
            }
        } else {
            DEBUG("closed\n");
        }

        // Send SCI
        send_sci = true;
    }
    lid_state = new;

    if (send_sci) {
        // Send SCI 0x1B for lid event
        if (pmc_sci(&PMC_1, 0x1B)) {
            send_sci = false;
        }
    }
}
