// SPDX-License-Identifier: GPL-3.0-only

#include <arch/delay.h>
#include <arch/time.h>
#include <board/acpi.h>
#include <board/fan.h>
#include <board/gpio.h>
#include <board/keymap.h>
#include <board/kbc.h>
#include <board/kbled.h>
#include <board/kbscan.h>
#include <board/lid.h>
#include <board/pmc.h>
#include <board/power.h>
#include <common/macro.h>
#include <common/debug.h>

// Default to not n-key rollover
#ifndef KM_NKEY
#define KM_NKEY 0
#endif // KM_NKEY

// Debounce time in milliseconds
#define DEBOUNCE_DELAY 10

bool kbscan_fn_held = false;
bool kbscan_esc_held = false;

bool kbscan_enabled = false;
uint16_t kbscan_repeat_period = 91;
uint16_t kbscan_repeat_delay = 500;

uint8_t kbscan_matrix[KM_OUT] = { 0 };

uint8_t sci_extra = 0;

static inline bool matrix_position_is_esc(uint8_t row, uint8_t col) {
    return (row == MATRIX_ESC_OUTPUT) && (col == MATRIX_ESC_INPUT);
}

static inline bool matrix_position_is_fn(uint8_t row, uint8_t col) {
    return (row == MATRIX_FN_OUTPUT) && (col == MATRIX_FN_INPUT);
}

// Assert the specified column for reading the row.
static void kbscan_set_column(uint8_t col) {
    // Assert the specific bit corresponding to the column.
    uint32_t colbit = ~BIT(col);
    KSOL = colbit & 0xFF;
    KSOH1 = (colbit >> 8) & 0xFF;
    KSOH2 = (colbit >> 16) & 0x03;

    // Wait for matrix to stabilize
    delay_ticks(20);
}

// Disable reading from all columns.
static void kbscan_disable_reading(void) {
    KSOL = 0xFF;
    KSOH1 = 0xFF;
    KSOH2 = 0x3;

    // No need to wait for matrix to stabilize as a read won't happen.
}

// Initialize the Keyboard Matrix Scan Controller in KBS (Normal) mode for
// reading keyboard input.
void kbscan_init(void) {
    // KSO[15:0]: Enable pull-up, set to KBS mode, open-drain
    // NOTE: KSO[17:16] ALT mode and pull-up configured by GPCRC
    KSOCTRL = BIT(2) | BIT(0);
    KSOHGCTRL = 0;
    KSOLGCTRL = 0;

    kbscan_disable_reading();

    // KSI[7:0]: Enable pull-up, set to KBS mode
    KSICTRLR = BIT(2);
    KSIGCTRL = 0;
}

// Read the state of the row for the selected column.
static inline uint8_t kbscan_get_row(void) {
    // Invert KSI for positive logic of pressed keys.
    return ~KSI;
}

#if KM_NKEY
static bool kbscan_row_has_ghost(uint8_t *matrix, uint8_t col) {
    (void)matrix;
    (void)col;
    return false;
}
#else // KM_NKEY
static inline bool popcount_more_than_one(uint8_t rowdata) {
    return rowdata & (rowdata - 1);
}

static bool kbscan_row_has_ghost(uint8_t *matrix, uint8_t col) {
    uint8_t rowdata = matrix[col];

    // No ghosts exist when less than 2 keys in the row are active.
    if (!popcount_more_than_one(rowdata)) {
        return false;
    }

    // Check against other rows to see if more than one column matches.
    for (uint8_t i = 0; i < KM_OUT; i++) {
        if (i == col) {
            continue;
        }

        uint8_t common = rowdata & matrix[i];
        if (popcount_more_than_one(common)) {
            return true;
        }
    }

    return false;
}
#endif // KM_NKEY

static void hardware_hotkey(uint16_t key) {
    switch (key) {
    case K_DISPLAY_TOGGLE:
        gpio_set(&BKL_EN, !gpio_get(&BKL_EN));
        break;
    case K_CAMERA_TOGGLE:
        gpio_set(&CCD_EN, !gpio_get(&CCD_EN));
        break;
    case K_FAN_TOGGLE:
        fan_max = !fan_max;
        break;
    case K_KBD_BKL:
        kbled_hotkey_step();
        break;
    case K_KBD_COLOR:
        if (acpi_ecos != EC_OS_FULL)
            kbled_hotkey_color();
        break;
    case K_KBD_DOWN:
        if (acpi_ecos != EC_OS_FULL)
            kbled_hotkey_down();
        break;
    case K_KBD_UP:
        if (acpi_ecos != EC_OS_FULL)
            kbled_hotkey_up();
        break;
    case K_KBD_TOGGLE:
        if (acpi_ecos != EC_OS_FULL)
            kbled_hotkey_toggle();
        break;
    }
}

bool kbscan_press(uint16_t key, bool pressed, uint8_t *layer) {
    // Wake from sleep on keypress
    if (pressed && (power_state == POWER_STATE_S3)) {
        pmc_swi();
    }

    if (key == K_FNLOCK && pressed) {
        DEBUG("Toggling FnLock\n");
        keymap_fnlock ^= 1;
        return true;
    }

    switch (key & KT_MASK) {
    case (KT_NORMAL):
        if (kbscan_enabled) {
            kbc_scancode(key, pressed);
        }
        break;
    case (KT_FN):
        if (layer != NULL) {
            if (pressed)
                *layer = 1;
            else
                *layer = 0;
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
                    kbc_scancode(K_LEFT_SUPER, true);
                    kbc_scancode(K_P, true);
                    kbc_scancode(K_P, false);
                } else {
                    kbc_scancode(K_LEFT_SUPER, false);
                }
            }
            break;
        case COMBO_PRINT_SCREEN:
            if (kbscan_enabled) {
                if (pressed) {
                    kbc_scancode(KF_E0 | 0x12, true);
                    kbc_scancode(KF_E0 | 0x7C, true);
                } else {
                    kbc_scancode(KF_E0 | 0x7C, false);
                    kbc_scancode(KF_E0 | 0x12, false);
                }
            }
            break;
        case COMBO_PAUSE:
            if (kbscan_enabled) {
                if (pressed) {
                    kbc_scancode(0xE1, true);
                    kbc_scancode(0x14, true);
                    kbc_scancode(0x77, true);
                    kbc_scancode(0xE1, true);
                    kbc_scancode(0x14, false);
                    kbc_scancode(0x77, false);
                }
            }
            break;
        }
        break;
    case (KT_SCI):
        if (pressed) {
            // Send SCI if ACPI OS is loaded
            if (acpi_ecos != EC_OS_NONE) {
                uint8_t sci = (uint8_t)(key & 0xFF);
                if (!pmc_sci(&PMC_1, sci)) {
                    // In the case of ignored SCI, reset bit
                    return false;
                }
            }

            // Handle hardware hotkeys
            hardware_hotkey(key);
        }
        break;
    case (KT_SCI_EXTRA):
        if (pressed) {
            // Send SCI if ACPI OS is loaded
            if (acpi_ecos != EC_OS_NONE) {
                uint8_t sci = SCI_EXTRA;
                sci_extra = (uint8_t)(key & 0xFF);
                if (!pmc_sci(&PMC_1, sci)) {
                    // In the case of ignored SCI, reset bit
                    return false;
                }
            }

            // Handle hardware hotkeys
            hardware_hotkey(key);
        }
        break;
    }
    return true;
}

static inline bool key_should_repeat(uint16_t key) {
    switch (key) {
    case K_AIRPLANE_MODE:
    case K_CAMERA_TOGGLE:
    case K_DISPLAY_TOGGLE:
    case K_FAN_TOGGLE:
    case K_FNLOCK:
    case K_KBD_BKL:
    case K_KBD_COLOR:
    case K_KBD_TOGGLE:
    case K_MIC_MUTE:
    case K_PAUSE:
    case K_SUSPEND:
    case K_TOUCHPAD:
        return false;
    }

    return true;
}

void kbscan_event(void) {
    static uint8_t kbscan_layer = 0;
    uint8_t layer = kbscan_layer;
    static uint8_t kbscan_last_layer[KM_OUT][KM_IN] = { { 0 } };
    uint8_t matrix_curr[KM_OUT];

    static bool debounce = false;
    static uint32_t debounce_time = 0;

    static bool repeat = false;
    static uint16_t repeat_key = 0;
    static uint32_t repeat_key_time = 0;

    // If debounce complete
    if (debounce) {
        uint32_t time = time_get();
        if ((time - debounce_time) >= DEBOUNCE_DELAY) {
            // Debounce time elapsed: Read new state
            debounce = false;
        } else {
            // If still debouncing, don't do anything.
            return;
        }
    }

    // Read the current state of the hardware matrix
    for (uint8_t i = 0; i < KM_OUT; i++) {
        if (!lid_state) {
            // Report all keys as released when lid is closed
            matrix_curr[i] = 0;
        } else {
            kbscan_set_column(i);
            matrix_curr[i] = kbscan_get_row();
        }
    }
    kbscan_disable_reading();

    for (uint8_t i = 0; i < KM_OUT; i++) {
        uint8_t new = matrix_curr[i];
        uint8_t last = kbscan_matrix[i];
        if (new != last) {
            if (kbscan_row_has_ghost(matrix_curr, i)) {
                continue;
            }

            // Some key has changed state: Start debounce
            if (!debounce) {
                debounce = true;
                debounce_time = time_get();
            }

            // A key was pressed or released
            for (uint8_t j = 0; j < KM_IN; j++) {
                // clang-format off
                bool new_b = new & BIT(j);
                bool last_b = last & BIT(j);
                // clang-format on
                if (new_b != last_b) {
                    bool reset = false;

                    // Check keys used for config reset
                    if (matrix_position_is_esc(i, j))
                        kbscan_esc_held = new_b;
                    if (matrix_position_is_fn(i, j))
                        kbscan_fn_held = new_b;

                    // Handle key press/release
                    if (new_b) {
                        // On a press, cache the layer the key was pressed on
                        kbscan_last_layer[i][j] = kbscan_layer;
                    }
                    uint8_t key_layer = kbscan_last_layer[i][j];
                    uint16_t key = 0;
                    keymap_get(key_layer, i, j, &key);
                    if (key) {
                        DEBUG("KB %d, %d, %d = 0x%04X, %d\n", i, j, key_layer, key, new_b);
                        if (!kbscan_press(key, new_b, &layer)) {
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

                    // Reset bit to last state
                    if (reset) {
                        if (last_b) {
                            new |= BIT(j);
                        } else {
                            new &= ~BIT(j);
                        }
                    }
                }
            }

            kbscan_matrix[i] = new;
        } else if (new &&repeat_key != 0 && key_should_repeat(repeat_key)) {
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
}
