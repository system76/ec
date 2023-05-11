// SPDX-License-Identifier: GPL-3.0-only

// German layout

#include <board/keymap.h>

// clang-format off
uint16_t __code KEYMAP[KM_LAY][KM_OUT][KM_IN] = {
LAYOUT(
    K_ESC, K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10, K_F11, K_F12, K_PRINT_SCREEN, K_INSERT, K_DEL, K_HOME, K_END, K_PGUP, K_PGDN,
    K_TICK, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9, K_0, K_SZ, K_BACKTICK, K_BKSP, K_NUM_LOCK, K_NUM_SLASH, K_NUM_ASTERISK, K_NUM_MINUS,
    K_TAB, K_Q, K_W, K_E, K_R, K_T, K_Z, K_U, K_I, K_O, K_P, K_UE, K_PLUS, K_ENTER, K_NUM_7, K_NUM_8, K_NUM_9, K_NUM_PLUS,
    K_CAPS, K_A, K_S, K_D, K_F, K_G, K_H, K_J, K_K, K_L, K_OE, K_AE, K_HASH, K_NUM_4, K_NUM_5, K_NUM_6,
    K_LEFT_SHIFT, K_Y, K_X, K_C, K_V, K_B, K_N, K_M, K_COMMA, K_DOT, K_HYPHEN, K_RIGHT_SHIFT, K_UP, K_NUM_1, K_NUM_2, K_NUM_3, K_NUM_ENTER,
    K_LEFT_CTRL, KT_FN, K_LEFT_SUPER, K_LEFT_ALT, K_SPACE, K_RIGHT_ALT, K_APP, K_RIGHT_CTRL, K_LEFT, K_DOWN, K_RIGHT, K_NUM_0, K_NUM_PERIOD
),
LAYOUT(
 ___ /*ESC*/, K_TOUCHPAD, K_DISPLAY_TOGGLE, K_MUTE, ___ /*F4*/, K_VOLUME_DOWN, K_VOLUME_UP, 
    #if CONFIG_MUX_COMBO
        K_MUX_TOGGLE
    #else
        K_DISPLAY_MODE
    #endif
    , K_BRIGHTNESS_DOWN, K_BRIGHTNESS_UP, K_CAMERA_TOGGLE, K_AIRPLANE_MODE, K_SUSPEND, ___ /*PRINT*/, K_SCROLL_LOCK, ___ /*K_DEL*/ , K_RESET, K_SHUTDOWN, K_PAUSE , K_PAUSE,
    K_PLAY_PAUSE, K_FAN_TOGGLE, ___ /*2*/ , ___ /*3*/ , ___ /*4*/ , ___ /*5*/ , ___ /*6*/ , ___ /*7*/ , ___ /*8*/ , ___ /*9*/ , ___ /*0*/ , ___ /*SZ*/ , ___ /*BACKTICK*/ , ___ /*BKSP*/ , ___ /*NUM_LOCK*/ , K_KBD_COLOR, K_KBD_TOGGLE, K_KBD_DOWN,
    ___ /*TAB*/ , ___ /*Q*/ , ___ /*W*/ , ___ /*E*/ , ___ /*R*/ , ___ /*T*/ , ___ /*Z*/ , ___ /*U*/ , ___ /*I*/ , ___ /*O*/ , ___ /*P*/ , ___ /*UE*/ , ___ /*PLUS*/ , ___ /*ENTER*/ , ___ /*7*/ , ___ /*8*/ , ___ /*9*/ , K_KBD_UP,
    ___ /*CAPS*/ , ___ /*A*/ , ___ /*S*/ , ___ /*D*/ , ___ /*F*/ , ___ /*G*/ , ___ /*H*/ , ___ /*J*/ , ___ /*K*/ , ___ /*L*/ , ___ /*OE*/ , ___ /*AE*/ , ___ /*HASH*/ , ___ /*4*/ , ___ /*5*/ , ___ /*6*/ ,
    ___ /*LEFT_SHIFT*/ , ___ /*Y*/ , ___ /*X*/ , ___ /*C*/ , ___ /*V*/ , ___ /*B*/ , ___ /*N*/ , ___ /*M*/ , ___ /*COMMA*/ , ___ /*DOT*/ , ___ /*HYPHEN*/ , ___ /*RIGHT_SHIFT*/ , ___ /*UP*/ , ___ /*1*/ , ___ /*2*/ , ___ /*3*/ , ___ /*ENTER*/ ,
    ___ /*LEFT_CTRL*/ , KT_FN, ___ /*LEFT_SUPER*/ , ___ /*LEFT_ALT*/ , ___ /*SPACE*/ , ___ /*RIGHT_ALT*/ , ___ /*APP*/ , ___ /*RIGHT_CTRL*/ , K_MEDIA_PREV, ___ /*DOWN*/ , K_MEDIA_NEXT, ___ /*0*/ , ___ /*PERIOD*/ 
)
};
// clang-format on
