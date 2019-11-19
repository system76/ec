// Jeremy's layout - http://www.keyboard-layout-editor.com/#/gists/43e77d2570f1c1c210bbdd69c106f961

#define K(V) {V, V}

uint16_t __code KEYMAP[KM_OUT][KM_IN][KM_LAY] = {
    {   //  0
        K(0), // 0
        K(0), // 1
        K(0), // 2
        K(0), // 3
        K(0), // 4
        K(0), // 5
        K(K_LEFT_ALT), // 6
        K(0), // 7
    },
    {   //  1
        K(0), // 0
        K(0), // 1
        K(0), // 2
        K(0), // 3
        K(0), // 4
        K(0), // 5
        K(K_LEFT_CTRL), // 6
        K(K_RIGHT_CTRL), // 7
    },
    {   //  2
        K(0), // 0
        K(0), // 1
        K(0), // 2
        K(0), // 3
        K(0), // 4
        K(0), // 5
        K(K_LEFT_SUPER), // 6
        K(K_FN), // 7
    },
    {   //  3
        K(0), // 0
        K(0), // 1
        K(0), // 2
        K(0), // 3
        K(0), // 4
        K(0), // 5
        K(K_LEFT_SHIFT), // 6
        K(K_RIGHT_SHIFT), // 7
    },
    {   //  4
        K(K_X), // 0
        K(K_Z), // 1
        K(K_FN), // 2
        K(K_A), // 3
        K(K_TAB), // 4
        K(0), // 5
        {K_SPACE, K_BKSP}, // 6
        K(K_Q), // 7
    },
    {   //  5
        K(K_V), // 0
        K(K_C), // 1
        K(0), // 2
        K(K_D), // 3
        K(0), // 4
        K(0), // 5
        K(0), // 6
        K(K_E), // 7
    },
    {   //  6
        K(K_FN), // 0
        K(K_B), // 1
        K(K_F), // 2
        K(K_G), // 3
        K(K_R), // 4
        K(K_T), // 5
        K(0), // 6
        K(0), // 7
    },
    {   //  7
        K(0), // 0
        {K_N, K_END}, // 1
        {K_H, K_LEFT}, // 2
        K(K_Y), // 3
        K(K_2), // 4
        K(K_1), // 5
        K(K_TICK), // 6
        K(K_ESC), // 7
    },
    {   //  8
        {K_I, K_HOME}, // 0
        K(K_M), // 1
        {K_U, K_PGUP}, // 2
        K(K_5), // 3
        K(K_4), // 4
        K(K_3), // 5
        K(K_F2), // 6
        K(K_F1), // 7
    },
    {   //  9
        K(K_W), // 0
        {K_J, K_DOWN}, // 1
        {K_K, K_UP}, // 2
        K(0), // 3
        K(K_7), // 4
        K(K_6), // 5
        K(K_F4), // 6
        {K_F3, K_MUTE}, // 7
    },
    {   // 10
        K(K_S), // 0
        K(K_COMMA), // 1
        {K_L, K_RIGHT}, // 2
        {K_O, K_PGDN}, // 3
        K(K_9), // 4
        K(K_8), // 5
        {K_F6, K_VOLUME_UP}, // 6
        {K_F5, K_VOLUME_DOWN}, // 7
    },
    {   // 11
        K(0), // 0
        K(K_UP), // 1
        K(K_PERIOD), // 2
        {K_SEMICOLON, K_INSERT}, // 3
        K(K_P), // 4
        K(K_0), // 5
        K(K_F8), // 6
        K(K_F7), // 7
    },
    {   // 12
        K(0), // 0
        K(0), // 1
        K(K_SLASH), // 2
        {K_QUOTE, K_DEL}, // 3
        K(K_BRACE_OPEN), // 4
        K(K_MINUS), // 5
        K(K_F10), // 6
        K(K_F9), // 7
    },
    {   // 13
        K(0), // 0
        K(0), // 1
        K(0 /* TODO: PrtSc/SysRq */), // 2
        K(K_BRACE_CLOSE), // 3
        K(K_EQUALS), // 4
        K(K_RIGHT), // 5
        K(0 /* TODO: Pause/Break */), // 6
        K(K_F11), // 7
    },
    {   // 14
        K(0), // 0
        K(K_BACKSLASH), // 1
        K(K_LEFT), // 2
        {K_BKSP, K_DEL}, // 3
        K(K_PGUP), // 4
        K(K_INSERT), // 5
        K(K_HOME), // 6
        K(K_DEL), // 7
    },
    {   // 15
        K(K_RIGHT_SUPER), // 0
        K(0), // 1
        K(K_ENTER), // 2
        K(0), // 3
        K(K_DOWN), // 4
        K(K_F12), // 5
        K(K_END), // 6
        K(K_PGDN), // 7
    },
};
