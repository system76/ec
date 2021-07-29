// SPDX-License-Identifier: GPL-3.0-only

#ifndef _COMMON_KEYMAP_H
#define _COMMON_KEYMAP_H

#include <stdbool.h>
#include <stdint.h>

// Keymap defined by board
#if defined(KM_LAY) && defined(KM_OUT) && defined(KM_IN)
    extern uint16_t __code KEYMAP[KM_LAY][KM_OUT][KM_IN];
    extern uint16_t __xdata DYNAMIC_KEYMAP[KM_LAY][KM_OUT][KM_IN];
    #define HAVE_KEYMAP 1
#else
    #define HAVE_KEYMAP 0
#endif

#if HAVE_KEYMAP
    // Initialize the dynamic keymap
    void keymap_init(void);
    // Set the dynamic keymap to the default keymap
    void keymap_load_default(void);
    // Erase dynamic keymap in flash
    bool keymap_erase_config(void);
    // Load dynamic keymap from flash
    bool keymap_load_config(void);
    // Save dynamic keymap to flash
    bool keymap_save_config(void);
    // Get a keycode from the dynamic keymap
    bool keymap_get(int layer, int output, int input, uint16_t * value);
    // Set a keycode in the dynamic keymap
    bool keymap_set(int layer, int output, int input, uint16_t value);
#endif

// Translate a keycode from PS/2 set 2 to PS/2 set 1
uint16_t keymap_translate(uint16_t key);

// Helper definition for empty key
#define ___ (0x0000)
#define K_NONE (0x0000)

// Key types
#define KT_MASK (0xF000)

// Normal keys
#define KT_NORMAL (0x0000)

// Layer selection
#define KT_FN (0x1000)

// Combinations
#define KT_COMBO (0x2000)

#define COMBO_DISPLAY_MODE 0
#define K_DISPLAY_MODE (KT_COMBO | COMBO_DISPLAY_MODE)
#define COMBO_PRINT_SCREEN 1
#define K_PRINT_SCREEN (KT_COMBO | COMBO_PRINT_SCREEN)

// SCI
#define KT_SCI (0x4000)

#define SCI_DISPLAY_TOGGLE (0x0B)
#define K_DISPLAY_TOGGLE (KT_SCI | SCI_DISPLAY_TOGGLE)
#define SCI_BRIGHTNESS_DOWN (0x11)
#define K_BRIGHTNESS_DOWN (KT_SCI | SCI_BRIGHTNESS_DOWN)
#define SCI_BRIGHTNESS_UP (0x12)
#define K_BRIGHTNESS_UP (KT_SCI | SCI_BRIGHTNESS_UP)
#define SCI_CAMERA_TOGGLE (0x13)
#define K_CAMERA_TOGGLE (KT_SCI | SCI_CAMERA_TOGGLE)
#define SCI_AIRPLANE_MODE (0x14)
#define K_AIRPLANE_MODE (KT_SCI | SCI_AIRPLANE_MODE)
#define SCI_SUSPEND (0x15)
#define K_SUSPEND (KT_SCI | SCI_SUSPEND)
#define SCI_EXTRA (0x50)

// Extra SCI layer for keyboard backlight control
#define KT_SCI_EXTRA (0x8000)

#define SCI_EXTRA_KBD_COLOR (0x80)
#define K_KBD_COLOR (KT_SCI_EXTRA | SCI_EXTRA_KBD_COLOR)
#define SCI_EXTRA_KBD_DOWN (0x81)
#define K_KBD_DOWN (KT_SCI_EXTRA | SCI_EXTRA_KBD_DOWN)
#define SCI_EXTRA_KBD_UP (0x82)
#define K_KBD_UP (KT_SCI_EXTRA | SCI_EXTRA_KBD_UP)
#define SCI_EXTRA_KBD_BKL (0x8A)
#define K_KBD_BKL (KT_SCI_EXTRA | SCI_EXTRA_KBD_BKL)
#define SCI_EXTRA_KBD_TOGGLE (0x9F)
#define K_KBD_TOGGLE (KT_SCI_EXTRA | SCI_EXTRA_KBD_TOGGLE)
#define SCI_EXTRA_FAN_TOGGLE (0xF2)
#define K_FAN_TOGGLE (KT_SCI_EXTRA | SCI_EXTRA_FAN_TOGGLE)

// See http://www.techtoys.com.hk/Downloads/Download/Microchip/PS2_driver/ScanCode.pdf

// Should send 0xE0 before scancode bytes
#define KF_E0 (0x0100)

// Hotkeys

#define K_PLAY_PAUSE (KF_E0 | 0x34)
#define K_MUTE (KF_E0 | 0x23)
#define K_VOLUME_DOWN (KF_E0 | 0x21)
#define K_VOLUME_UP (KF_E0 | 0x32)
// More media keys
#define K_MEDIA_NEXT (KF_E0 | 0x4D)
#define K_MEDIA_PREV (KF_E0 | 0x15)
// Custom scancode
#define K_TOUCHPAD (KF_E0 | 0x63)

// Function keys

#define K_F1 (0x05)
#define K_F2 (0x06)
#define K_F3 (0x04)
#define K_F4 (0x0C)
#define K_F5 (0x03)
#define K_F6 (0x0B)
#define K_F7 (0x83)
#define K_F8 (0x0A)
#define K_F9 (0x01)
#define K_F10 (0x09)
#define K_F11 (0x78)
#define K_F12 (0x07)

// Number keys

#define K_0 (0x45)
#define K_1 (0x16)
#define K_2 (0x1E)
#define K_3 (0x26)
#define K_4 (0x25)
#define K_5 (0x2E)
#define K_6 (0x36)
#define K_7 (0x3D)
#define K_8 (0x3E)
#define K_9 (0x46)

// Letter keys

#define K_A (0x1C)
#define K_B (0x32)
#define K_C (0x21)
#define K_D (0x23)
#define K_E (0x24)
#define K_F (0x2B)
#define K_G (0x34)
#define K_H (0x33)
#define K_I (0x43)
#define K_J (0x3B)
#define K_K (0x42)
#define K_L (0x4B)
#define K_M (0x3A)
#define K_N (0x31)
#define K_O (0x44)
#define K_P (0x4D)
#define K_Q (0x15)
#define K_R (0x2D)
#define K_S (0x1B)
#define K_T (0x2C)
#define K_U (0x3C)
#define K_V (0x2A)
#define K_W (0x1D)
#define K_X (0x22)
#define K_Y (0x35)
#define K_Z (0x1A)

// Special keys

// Escape key
#define K_ESC (0x76)

//TODO: Pause/break, sys request

// Scroll lock
#define K_SCROLL_LOCK (0x7E)

// Tick/tilde key
#define K_TICK (0x0E)
// Minus/underline key
#define K_MINUS (0x4E)
// Equals/plus key
#define K_EQUALS (0x55)
// Backspace key
#define K_BKSP (0x66)

// Tab key
#define K_TAB (0x0D)
// Bracket open key
#define K_BRACE_OPEN (0x54)
// Bracket close key
#define K_BRACE_CLOSE (0x5B)
// Backslash/pipe key
#define K_BACKSLASH (0x5D)

// Capslock
#define K_CAPS (0x58)
// Semicolon key
#define K_SEMICOLON (0x4C)
// Quote key
#define K_QUOTE (0x52)
// Enter key
#define K_ENTER (0x5A)

// Left shift
#define K_LEFT_SHIFT (0x12)
// Comma key
#define K_COMMA (0x41)
// Period key
#define K_PERIOD (0x49)
// Slash key
#define K_SLASH (0x4A)
// Right shift
#define K_RIGHT_SHIFT (0x59)

// Left control key
#define K_LEFT_CTRL (0x14)
// Left super key
#define K_LEFT_SUPER (KF_E0 | 0x1F)
// Left alt key
#define K_LEFT_ALT (0x11)
// Space key
#define K_SPACE (0x29)
// Right alt key
#define K_RIGHT_ALT (KF_E0 | 0x11)
// Right super key
#define K_RIGHT_SUPER (KF_E0 | 0x27)
// Application key
#define K_APP (KF_E0 | 0x2F)
// Right control key
#define K_RIGHT_CTRL (KF_E0 | 0x14)

// Arrow keys and related

// Insert key
#define K_INSERT (KF_E0 | 0x70)
// Delete key
#define K_DEL (KF_E0 | 0x71)
// Home key
#define K_HOME (KF_E0 | 0x6C)
// End key
#define K_END (KF_E0 | 0x69)
// Page up key
#define K_PGUP (KF_E0 | 0x7D)
// Page down key
#define K_PGDN (KF_E0 | 0x7A)

#define K_UP (KF_E0 | 0x75)
#define K_LEFT (KF_E0 | 0x6B)
#define K_DOWN (KF_E0 | 0x72)
#define K_RIGHT (KF_E0 | 0x74)

// Numpad

#define K_NUM_LOCK (0x77)
#define K_NUM_SLASH (KF_E0 | 0x4A)
#define K_NUM_ASTERISK (0x7C)
#define K_NUM_MINUS (0x7B)
#define K_NUM_PLUS (0x79)
#define K_NUM_PERIOD (0x71)
#define K_NUM_ENTER (KF_E0 | 0x5A)
#define K_NUM_0 (0x70)
#define K_NUM_1 (0x69)
#define K_NUM_2 (0x72)
#define K_NUM_3 (0x7A)
#define K_NUM_4 (0x6B)
#define K_NUM_5 (0x73)
#define K_NUM_6 (0x74)
#define K_NUM_7 (0x6C)
#define K_NUM_8 (0x75)
#define K_NUM_9 (0x7D)

// International keys

#define K_INT_1 (0x61)
#define K_INT_2 (0x5D)

#endif // _COMMON_KEYMAP_H
