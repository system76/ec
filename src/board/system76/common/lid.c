// SPDX-License-Identifier: GPL-3.0-only

#include <arch/delay.h>
#include <board/acpi.h>
#include <board/gpio.h>
#include <board/lid.h>
#include <board/pmc.h>
#include <common/debug.h>

// Default closed to prevent spurious power on
bool lid_state = false;
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
                pmc_swi();
                lid_wake = false;
            }
        } else {
            DEBUG("closed\n");
        }

        // Send SCI if ACPI OS is loaded
        send_sci = true;
    }
    lid_state = new;

    if (send_sci) {
        // Send SCI 0x1B for lid event if ACPI OS is loaded
        if (acpi_ecos != EC_OS_NONE) {
            if (pmc_sci(&PMC_1, 0x1B)) {
                send_sci = false;
            }
        }
    }
}
