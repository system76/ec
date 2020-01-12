// Default layout

#define ___ 0

#define LAYOUT( \
    K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, K0E, K0F, K0G, \
    K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, \
    K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, K2D, \
    K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C, \
    K40, K41, K42, K43, K44, K45, K46, K47, K48, K49, K4A, K4B, \
    K50, K51, K52, K53, K54, K55, K56, K57, K58, K59, \
    K60, K61, K62 \
) { \
    { ___, ___, ___, ___, ___, ___, K52, ___ }, \
    { ___, ___, ___, ___, ___, ___, K50, K56 }, \
    { ___, ___, ___, ___, ___, ___, K53, K55 }, \
    { ___, ___, ___, ___, ___, ___, K40, K4B }, \
    { K42, K41, K30, K31, K20, ___, K54, K21 }, \
    { K44, K43, ___, K33, ___, ___, ___, K23 }, \
    { K51, K45, K34, K35, K24, K25, ___, ___ }, \
    { ___, K46, K36, K26, K12, K11, K10, K00 }, \
    { K28, K47, K27, K15, K14, K13, K02, K01 }, \
    { K22, K37, K38, ___, K17, K16, K04, K03 }, \
    { K32, K48, K39, K29, K19, K18, K06, K05 }, \
    { ___, K58, K49, K3A, K2A, K1A, K08, K07 }, \
    { ___, ___, K4A, K3B, K2B, K1B, K0A, K09 }, \
    { ___, ___, K0D, K2C, K1C, K62, K0E, K0B }, \
    { ___, K2D, K60, K1D, K57, K0F, ___, K0G }, \
    { ___, ___, K3C, ___, K61, K0C, ___, K59 } \
}

uint16_t __code KEYMAP[KM_LAY][KM_OUT][KM_IN] = {
LAYOUT(
    K_ESC, K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10, K_F11, K_F12, K_HOME, K_END, 0 /*prtsc*/, K_DEL,
    K_TICK, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9, K_0, K_MINUS, K_EQUALS, K_BKSP,
    K_TAB, K_Q, K_W, K_E, K_R, K_T, K_Y, K_U, K_I, K_O, K_P, K_BRACE_OPEN, K_BRACE_CLOSE, K_BACKSLASH,
    K_CAPS, K_A, K_S, K_D, K_F, K_G, K_H, K_J, K_K, K_L, K_SEMICOLON, K_QUOTE, K_ENTER,
    K_LEFT_SHIFT, K_Z, K_X, K_C, K_V, K_B, K_N, K_M, K_COMMA, K_PERIOD, K_SLASH, K_RIGHT_SHIFT,
    K_LEFT_CTRL, KT_FN, K_LEFT_SUPER, K_LEFT_ALT, K_SPACE, K_RIGHT_ALT, K_RIGHT_CTRL, K_PGUP, K_UP, K_PGDN,
    K_LEFT, K_DOWN, K_RIGHT
),
LAYOUT(
    K_ESC, K_TOUCHPAD, K_F2, K_MUTE, K_F4, K_VOLUME_DOWN, K_VOLUME_UP, K_F7, KT_SCI | SCI_BRIGHTNESS_DOWN, KT_SCI | SCI_BRIGHTNESS_UP, K_F10, KT_SCI | SCI_AIRPLANE_MODE, KT_SCI | SCI_SUSPEND, K_HOME, K_END, 0 /*prtsc*/, K_DEL,
    K_PLAY_PAUSE, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9, K_0, K_MINUS, K_EQUALS, K_BKSP,
    K_TAB, K_Q, K_W, K_E, K_R, K_T, K_Y, K_U, K_I, K_O, K_P, K_BRACE_OPEN, K_BRACE_CLOSE, K_BACKSLASH,
    K_CAPS, K_A, K_S, K_D, K_F, K_G, K_H, K_J, K_K, K_L, K_SEMICOLON, K_QUOTE, K_ENTER,
    K_LEFT_SHIFT, K_Z, K_X, K_C, K_V, K_B, K_N, K_M, K_COMMA, K_PERIOD, K_SLASH, K_RIGHT_SHIFT,
    K_LEFT_CTRL, KT_FN, K_LEFT_SUPER, K_LEFT_ALT, K_SPACE, K_RIGHT_ALT, K_RIGHT_CTRL, K_PGUP, K_UP, K_PGDN,
    K_LEFT, K_DOWN, K_RIGHT
)
};
