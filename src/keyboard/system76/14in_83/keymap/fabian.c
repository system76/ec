// SPDX-License-Identifier: GPL-3.0-only

// # Fabian ISO layout
// Swapped FN and LEFT_CTRL (like on Thinkpads and Macbooks)
// Key right of left shift is shift (ANSI like, less strain)
// CapsLock is control
// FN + LEFT_SHIFT is keyboard backlight (in the dark, you don't want to search F4)

#include <board/keymap.h>

uint16_t __code KEYMAP[KM_LAY][KM_OUT][KM_IN] = {
// TODO: K0E=K_PAUSE (once defined in src/common/include/common/keymap.h)
LAYOUT_ISO(
    K_ESC, K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10, K_F11, K_F12, K_PRINT_SCREEN, K_PLAY_PAUSE, K_INSERT, K_DEL,
    K_TICK, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9, K_0, K_MINUS, K_EQUALS, K_BKSP,
    K_TAB, K_Q, K_W, K_E, K_R, K_T, K_Y, K_U, K_I, K_O, K_P, K_BRACE_OPEN, K_BRACE_CLOSE, K_ENTER,
    K_LEFT_CTRL, K_A, K_S, K_D, K_F, K_G, K_H, K_J, K_K, K_L, K_SEMICOLON, K_QUOTE, K_BACKSLASH,
    K_LEFT_SHIFT, K_LEFT_SHIFT, K_Z, K_X, K_C, K_V, K_B, K_N, K_M, K_COMMA, K_PERIOD, K_SLASH, K_RIGHT_SHIFT,
    KT_FN, K_LEFT_CTRL, K_LEFT_SUPER, K_LEFT_ALT, K_SPACE, K_RIGHT_ALT, K_RIGHT_CTRL, K_PGUP, K_UP, K_PGDN,
    K_LEFT, K_DOWN, K_RIGHT
),
// TODO: K0D=K_SYS_RQ, K0E=K_BREAK, K0G=K_SCROLL_LOCK (once defined in src/common/include/common/keymap.h)
LAYOUT_ISO(
    K_ESC, K_TOUCHPAD, K_DISPLAY_TOGGLE, K_MUTE, K_KBD_BKL, K_VOLUME_DOWN, K_VOLUME_UP, K_DISPLAY_MODE, K_BRIGHTNESS_DOWN, K_BRIGHTNESS_UP, K_CAMERA_TOGGLE, K_AIRPLANE_MODE, K_SUSPEND, K_PRINT_SCREEN, K_PLAY_PAUSE, K_NUM_LOCK, K_DEL,
    K_PLAY_PAUSE, K_FAN_TOGGLE, K_2, K_3, K_4, K_5, K_6, K_NUM_7, K_NUM_8, K_NUM_9, K_NUM_ASTERISK, K_MINUS, K_EQUALS, K_BKSP,
    K_TAB, K_Q, K_W, K_E, K_R, K_T, K_Y, K_NUM_4, K_NUM_5, K_NUM_6, K_NUM_SLASH, K_BRACE_OPEN, K_BRACE_CLOSE, K_ENTER,
    K_LEFT_CTRL, K_A, K_S, K_D, K_F, K_G, K_H, K_NUM_1, K_NUM_2, K_NUM_3, K_NUM_MINUS, K_QUOTE, K_BACKSLASH,
    K_KBD_BKL, K_LEFT_SHIFT, K_Z, K_X, K_C, K_V, K_B, K_N, K_NUM_0, K_COMMA, K_NUM_PERIOD, K_NUM_PLUS, K_RIGHT_SHIFT,
    KT_FN, K_LEFT_CTRL, K_LEFT_SUPER, K_LEFT_ALT, K_SPACE, K_RIGHT_ALT, K_APP, K_HOME, K_UP, K_END,
    K_LEFT, K_DOWN, K_RIGHT
)
};
