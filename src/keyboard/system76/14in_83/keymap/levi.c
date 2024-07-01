// SPDX-License-Identifier: GPL-3.0-only

// Default layout

#include <board/keymap.h>

// uncrustify:off
uint16_t __code KEYMAP[KM_LAY][KM_OUT][KM_IN] = {
LAYOUT(
    K_ESC, K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10, K_F11, K_F12, K_HOME, K_END, K_PRINT_SCREEN, K_DEL,
    K_TICK, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9, K_0, K_BRACE_OPEN, K_BRACE_CLOSE, K_BKSP,
    K_TAB, K_QUOTE, K_COMMA, K_PERIOD, K_P, K_Y, K_F, K_G, K_C, K_R, K_L, K_SLASH, K_EQUALS, K_BACKSLASH,
    K_LEFT_CTRL, K_A, K_O, K_E, K_U, K_I, K_D, K_H, K_T, K_N, K_S, K_MINUS, K_ENTER,
    K_LEFT_SHIFT, K_SEMICOLON, K_Q, K_J, K_K, K_X, K_B, K_M, K_W, K_V, K_Z, K_RIGHT_SHIFT,
    KT_FN, K_Z, K_LEFT_ALT, K_LEFT_SUPER, K_SPACE, K_RIGHT_ALT, K_RIGHT_CTRL, K_PGUP, K_UP, K_PGDN,
    K_LEFT, K_DOWN, K_RIGHT
),
LAYOUT(
    K_ESC, K_TOUCHPAD, K_DISPLAY_TOGGLE, K_MUTE, K_KBD_BKL, K_VOLUME_DOWN, K_VOLUME_UP, K_DISPLAY_MODE, K_BRIGHTNESS_DOWN, K_BRIGHTNESS_UP, K_CAMERA_TOGGLE, K_AIRPLANE_MODE, K_SUSPEND, K_HOME, K_END, K_PRINT_SCREEN, K_DEL,
    K_PLAY_PAUSE, K_FAN_TOGGLE, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9, K_0, K_MINUS, K_EQUALS, K_BKSP,
    K_TAB, K_HOME, K_UP, K_END, K_PGUP, K_Y, K_PGUP, K_HOME, K_UP, K_END, K_L, K_SLASH, K_EQUALS, K_BACKSLASH,
    K_LEFT_CTRL, K_LEFT, K_DOWN, K_RIGHT, K_PGDN, K_I, K_PGDN, K_LEFT, K_DOWN, K_RIGHT, K_S, K_MINUS, K_ENTER,
    K_LEFT_SHIFT, K_PLAY_PAUSE, K_MEDIA_PREV, K_MEDIA_NEXT, K_VOLUME_DOWN, K_VOLUME_UP, K_MUTE, K_M, K_W, K_V, K_Z, K_RIGHT_SHIFT,
    KT_FN, K_Z, K_LEFT_ALT, K_LEFT_SUPER, K_SPACE, K_RIGHT_ALT, K_RIGHT_CTRL, K_PGUP, K_UP, K_PGDN,
    K_LEFT, K_DOWN, K_RIGHT
)
};
// uncrustify:on
