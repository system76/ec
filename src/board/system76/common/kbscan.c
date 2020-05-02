#include <arch/delay.h>
#include <arch/time.h>
#include <board/gpio.h>
#include <board/kbc.h>
#include <board/kbled.h>
#include <board/kbscan.h>
#include <board/keymap.h>
#include <board/pmc.h>
#include <board/power.h>
#include <common/debug.h>

bool kbscan_enabled = false;
uint16_t kbscan_repeat_period = 91;
uint16_t kbscan_repeat_delay = 500;

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
#define DEBOUNCE_DELAY 15

static uint8_t kbscan_get_row(int i) {
    // Set current line as output
    if (i < 8) {
        KSOLGOEN = 1 << i;
        KSOHGOEN = 0;
#if KM_OUT >= 17
        GPCRC3 = GPIO_IN;
#endif
#if KM_OUT >= 18
        GPCRC5 = GPIO_IN;
#endif
    } else if (i < 16) {
        KSOLGOEN = 0;
        KSOHGOEN = 1 << (i - 8);
#if KM_OUT >= 17
        GPCRC3 = GPIO_IN;
#endif
#if KM_OUT >= 18
        GPCRC5 = GPIO_IN;
#endif
    } else if (i == 16) {
        KSOLGOEN = 0;
        KSOHGOEN = 0;
#if KM_OUT >= 17
        GPCRC3 = GPIO_OUT;
#endif
#if KM_OUT >= 18
        GPCRC5 = GPIO_IN;
#endif
    } else if (i == 17) {
        KSOLGOEN = 0;
        KSOHGOEN = 0;
#if KM_OUT >= 17
        GPCRC3 = GPIO_IN;
#endif
#if KM_OUT >= 18
        GPCRC5 = GPIO_OUT;
#endif
    }
#if KM_OUT >= 17
    GPDRC &= ~(1 << 3);
#endif
#if KM_OUT >= 18
    GPDRC &= ~(1 << 5);
#endif

    // TODO: figure out optimal delay
    delay_ticks(10);

    return ~KSI;
}

static inline bool popcount_more_than_one(uint8_t rowdata) {
    return rowdata & (rowdata - 1);
}

static uint8_t kbscan_get_real_keys(int row, uint8_t rowdata) {
    // Remove any "active" blanks from the matrix.
    uint8_t realdata = 0;
    for (uint8_t col = 0; col < KM_IN; col++) {
        if (KEYMAP[0][row][col] && (rowdata & (1 << col))) {
            realdata |=  1 << col;
        }
    }

    return realdata;
}

static bool kbscan_has_ghost_in_row(int row, uint8_t rowdata) {
    rowdata = kbscan_get_real_keys(row, rowdata);

    // No ghosts exist when  less than 2 keys in the row are active.
    if (!popcount_more_than_one(rowdata)) {
        return false;
    }

    // Check against other rows to see if more than one column matches.
    for (int i = 0; i < KM_OUT; i++) {
        uint8_t otherrow = kbscan_get_real_keys(i, kbscan_get_row(i));
        if (i != row && popcount_more_than_one(otherrow & rowdata)) {
            return true;
        }
    }

    return false;
}

bool kbscan_press(uint16_t key, bool pressed, uint8_t * layer) {
    if (pressed &&
        (power_state == POWER_STATE_S3 || power_state == POWER_STATE_DS3)) {
        gpio_set(&SWI_N, false);
        delay_ticks(10); //TODO: find correct delay
        gpio_set(&SWI_N, true);
    }

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
                            kbc_scancode(&KBC, K_LEFT_SUPER, true);
                            kbc_scancode(&KBC, K_P, true);
                            kbc_scancode(&KBC, K_P, false);
                        } else {
                            kbc_scancode(&KBC, K_LEFT_SUPER, false);
                        }
                    }
                    break;
                case COMBO_PRINT_SCREEN:
                    if (kbscan_enabled) {
                        if (pressed) {
                            kbc_scancode(&KBC, K_E0 | 0x12, true);
                            kbc_scancode(&KBC, K_E0 | 0x7C, true);
                        } else {
                            kbc_scancode(&KBC, K_E0 | 0x7C, false);
                            kbc_scancode(&KBC, K_E0 | 0x12, false);
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
    static uint8_t kbscan_last_layer[KM_OUT][KM_IN] = { { 0 } };
    static bool kbscan_ghost[KM_OUT] = { false };

    static bool debounce = false;
    static uint32_t debounce_time = 0;

    static bool repeat = false;
    static uint16_t repeat_key = 0;
    static uint32_t repeat_key_time = 0;

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
        uint8_t new = kbscan_get_row(i);
        uint8_t last = kbscan_last[i];
        if (new != last) {
            if (kbscan_has_ghost_in_row(i, new)) {
                kbscan_ghost[i] = true;
                continue;
            } else if (kbscan_ghost[i]) {
                kbscan_ghost[i] = false;
                // Debounce to allow remaining ghosts to settle.
                debounce = true;
            }

            // A key was pressed or released
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
                        if (new_b) {
                            // On a press, cache the layer the key was pressed on
                            kbscan_last_layer[i][j] = kbscan_layer;
                        }
                        uint8_t key_layer = kbscan_last_layer[i][j];
                        uint16_t key = keymap(i, j, key_layer);
                        if (key) {
                            DEBUG("KB %d, %d, %d = 0x%04X, %d\n", i, j, key_layer, key, new_b);
                            if(!kbscan_press(key, new_b, &layer)){
                                // In the case of ignored key press/release, reset bit
                                reset = true;
                            }

                            if (new_b) {
                                // New key pressed, update last key
                                repeat_key = key;
                                repeat_key_time = time_get();
                                repeat = false;
                            } else if (key == repeat_key) {
                                // Repeat key was released
                                repeat_key = 0;
                                repeat = false;
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
        } else if (new && repeat_key != 0) {
            // A key is being pressed
            uint32_t time = time_get();
            static uint32_t repeat_start = 0;

            if (!repeat) {
                if (time < repeat_key_time) {
                    // Overflow, reset repeat_key_time
                    repeat_key_time = time;
                } else if ((time - repeat_key_time) >= kbscan_repeat_delay) {
                    // Typematic repeat
                    repeat = true;
                    repeat_start = time;
                }
            }

            if (repeat) {
                if ((time - repeat_start) > kbscan_repeat_period) {
                    kbscan_press(repeat_key, true, &layer);
                    repeat_start = time;
                }
            }
        }
    }

    kbscan_layer = layer;

    // Reset all lines to inputs
    KSOLGOEN = 0;
    KSOHGOEN = 0;
#if KM_OUT >= 17
    GPCRC3 = GPIO_IN;
#endif
#if KM_OUT >= 18
    GPCRC5 = GPIO_IN;
#endif

    // TODO: figure out optimal delay
    delay_ticks(10);
}
