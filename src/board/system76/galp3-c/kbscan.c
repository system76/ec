#include <arch/delay.h>
#include <arch/time.h>
#include <board/gpio.h>
#include <board/kbc.h>
#include <board/kbled.h>
#include <board/kbscan.h>
#include <board/keymap.h>
#include <board/pmc.h>
#include <common/debug.h>

bool kbscan_enabled = false;

uint8_t sci_extra = 0;

void kbscan_init(void) {
    KSOCTRL = 0x05;
    KSICTRLR = 0x04;

    // Set all outputs to GPIO mode, low, and inputs
    KSOL = 0;
    KSOLGCTRL = 0xFF;
    KSOLGOEN = 0;
    KSOH1 = 0;
    KSOHGCTRL = 0xFF;
    KSOHGOEN = 0;
    KSOH2 = 0;
}

// Debounce time in milliseconds
#define DEBOUNCE_DELAY 20

bool kbscan_press(uint16_t key, bool pressed, uint8_t * layer) {
    switch (key & KT_MASK) {
        case (KT_NORMAL):
            if (kbscan_enabled) {
                kbc_scancode(&KBC, key, pressed);
            }
            break;
        case (KT_FN):
            if (layer != NULL) {
                if (pressed) *layer = 1;
                else *layer = 0;
            } else {
                // In the case no layer can be set, reset bit
                return false;
            }
            break;
        case (KT_COMBO):
            switch (key & 0xFF) {
                case COMBO_DISPLAY_MODE:
                    if (kbscan_enabled) {
                        if (pressed) {
                            kbc_scancode(&KBC, K_LEFT_SUPER, pressed);
                            kbc_scancode(&KBC, K_P, pressed);
                        } else {
                            kbc_scancode(&KBC, K_P, pressed);
                            kbc_scancode(&KBC, K_LEFT_SUPER, pressed);
                        }
                    }
                    break;
                case COMBO_PRINT_SCREEN:
                    if (kbscan_enabled) {
                        if (pressed) {
                            kbc_scancode(&KBC, K_E0 | 0x12, pressed);
                            kbc_scancode(&KBC, K_E0 | 0x7C, pressed);
                        } else {
                            kbc_scancode(&KBC, K_E0 | 0x7C, pressed);
                            kbc_scancode(&KBC, K_E0 | 0x12, pressed);
                        }
                    }
                    break;
            }
            break;
        case (KT_SCI):
            if (pressed) {
                uint8_t sci = (uint8_t)(key & 0xFF);

                // HACK FOR HARDWARE HOTKEYS
                switch (sci) {
                    case SCI_DISPLAY_TOGGLE:
                        gpio_set(&BKL_EN, !gpio_get(&BKL_EN));
                        break;
                    case SCI_CAMERA_TOGGLE:
                        gpio_set(&CCD_EN, !gpio_get(&CCD_EN));
                        break;
                }

                if (!pmc_sci(&PMC_1, sci)) {
                    // In the case of ignored SCI, reset bit
                    return false;
                }
            }
            break;
        case (KT_SCI_EXTRA):
            if (pressed) {
                uint8_t sci = SCI_EXTRA;
                sci_extra = (uint8_t)(key & 0xFF);
                
                // HACK FOR HARDWARE HOTKEYS
                switch (sci_extra) {
                    case SCI_EXTRA_KBD_BKL:
                        kbled_set(kbled_get() + 1);
                        break;
                }

                if (!pmc_sci(&PMC_1, sci)) {
                    // In the case of ignored SCI, reset bit
                    return false;
                }
            }
            break;
    }
    return true;
}

void kbscan_event(void) {
    static uint8_t kbscan_layer = 0;
    uint8_t layer = kbscan_layer;
    static uint8_t kbscan_last[KM_OUT] = { 0 };

    static bool debounce = false;
    static uint32_t debounce_time = 0;

    // If debounce complete
    if (debounce) {
        uint32_t time = time_get();
        //TODO: time test with overflow
        if (time < debounce_time) {
            // Overflow, reset debounce_time
            debounce_time = time;
        } else if (time >= (debounce_time + DEBOUNCE_DELAY)) {
            // Finish debounce
            debounce = false;
        }
    }

    int i;
    for (i = 0; i < KM_OUT; i++) {
        // Set current line as output
        if (i < 8) {
            KSOLGOEN = 0;
            KSOLGOEN = 1 << i;
        } else if (i < 16) {
            KSOLGOEN = 0;
            KSOHGOEN = 1 << (i - 8);
        } else if (i == 16) {
            KSOLGOEN = 0;
            KSOHGOEN = 0;
        } else if (i == 17) {
            KSOLGOEN = 0;
            KSOHGOEN = 0;
        }

        // TODO: figure out optimal delay
        delay_ticks(10);

        uint8_t new = ~KSI;
        uint8_t last = kbscan_last[i];
        if (new != last) {
            int j;
            for (j = 0; j < KM_IN; j++) {
                bool new_b = new & (1 << j);
                bool last_b = last & (1 << j);
                if (new_b != last_b) {
                    bool reset = false;

                    // If debouncing
                    if (debounce) {
                        // Debounce presses and releases
                        reset = true;
                    } else {
                        // Begin debounce
                        debounce = true;
                        debounce_time = time_get();

                        // Handle key press/release
                        uint16_t key = keymap(i, j, kbscan_layer);
                        if (key) {
                            DEBUG("KB %d, %d, %d = 0x%04X, %d\n", i, j, kbscan_layer, key, new_b);
                            if(!kbscan_press(key, new_b, &layer)){
                                // In the case of ignored key press/release, reset bit
                                reset = true;
                            }
                        } else {
                            WARN("KB %d, %d, %d missing\n", i, j, kbscan_layer);
                        }
                    }

                    // Reset bit to last state
                    if (reset) {
                        if (last_b) {
                            new |= (1 << j);
                        } else {
                            new &= ~(1 << j);
                        }
                    }
                }
            }

            kbscan_last[i] = new;
        }
    }

    if (layer != kbscan_layer) {
        //TODO: unpress keys before going to scratch rom

        // Unpress all currently pressed keys
        for (i = 0; i < KM_OUT; i++) {
            uint8_t new = 0;
            uint8_t last = kbscan_last[i];
            if (last) {
                int j;
                for (j = 0; j < KM_IN; j++) {
                    bool new_b = new & (1 << j);
                    bool last_b = last & (1 << j);
                    if (new_b != last_b) {
                        bool reset = false;

                        // Handle key press/release
                        uint16_t key = keymap(i, j, kbscan_layer);
                        if (key) {
                            DEBUG("KB %d, %d, %d = 0x%04X, %d\n", i, j, kbscan_layer, key, new_b);
                            if(!kbscan_press(key, new_b, NULL)){
                                // In the case of ignored key press/release, reset bit
                                reset = true;
                            }
                        } else {
                            WARN("KB %d, %d, %d missing\n", i, j, kbscan_layer);
                        }

                        // Reset bit to last state
                        if (reset) {
                            if (last_b) {
                                new |= (1 << j);
                            } else {
                                new &= ~(1 << j);
                            }
                        }
                    }
                }
            }

            kbscan_last[i] = new;
        }

        kbscan_layer = layer;
    }

    // Reset all lines to inputs
    KSOLGOEN = 0;
    KSOHGOEN = 0;

    // TODO: figure out optimal delay
    delay_ticks(10);
}
