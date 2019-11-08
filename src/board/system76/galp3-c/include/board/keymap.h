#ifndef _BOARD_KEYMAP_H
#define _BOARD_KEYMAP_H

#include <stdint.h>

// Keymap output pins
#define KM_OUT 16
// Keymap input pins
#define KM_IN 8
// Keymap layers (normal, Fn)
#define KM_LAY 2

uint16_t keymap(int output, int input, int layer);

uint16_t keymap_translate(uint16_t key);

// See http://www.techtoys.com.hk/Downloads/Download/Microchip/PS2_driver/ScanCode.pdf

// Should send 0xE0 before scancode bytes
#define K_E0 0x0100

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

//TODO: Print screen, scroll lock, pause

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
#define K_LEFT_SUPER (K_E0 | 0x1F)
// Left alt key
#define K_LEFT_ALT (0x11)
// Space key
#define K_SPACE (0x29)
// Right alt key
#define K_RIGHT_ALT (K_E0 | 0x11)
// Right super key
#define K_RIGHT_SUPER (K_E0 | 0x27)
// Application key
#define K_APP (K_E0 | 0x2F)
// Right control key
#define K_RIGHT_CTRL (K_E0 | 0x14)

// Arrow keys and related

// Insert key
#define K_INSERT (K_E0 | 0x70)
// Delete key
#define K_DEL (K_E0 | 0x71)
// Home key
#define K_HOME (K_E0 | 0x6C)
// End key
#define K_END (K_E0 | 0x69)
// Page up key
#define K_PGUP (K_E0 | 0x7D)
// Page down key
#define K_PGDN (K_E0 | 0x7A)

#define K_UP (K_E0 | 0x75)
#define K_LEFT (K_E0 | 0x6B)
#define K_DOWN (K_E0 | 0x72)
#define K_RIGHT (K_E0 | 0x74)

#endif // _BOARD_KEYMAP_H
