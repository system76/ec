// Default layout

#define K(V) {V, V}

uint16_t __code KEYMAP[KM_OUT][KM_IN][KM_LAY] = {
    {   //  0
        K(0), // 0
        K(0), // 1
        K(0), // 2
        K(0), // 3
        K(0), // 4
        K(0), // 5
        K(K_LEFT_CTRL), // 6
        K(K_RIGHT_CTRL), // 7
    },
    {   //  1
        K(0), // 0
        K(0), // 1
        K(0), // 2
        K(0), // 3
        K(0), // 4
        K(0), // 5
        K(K_LEFT_ALT), // 6
        K(K_RIGHT_ALT), // 7
    },
    {   //  2
        K(0), // 0
        K(0), // 1
        K(0), // 2
        K(0), // 3
        K(0), // 4
        K(0), // 5
        K(K_LEFT_SHIFT), // 6
        K(K_RIGHT_SHIFT), // 7
    },
    {   //  3
        K(K_LEFT_SUPER), // 0
        K(0), // 1
        K(0), // 2
        K(0), // 3
        K(0), // 4
        K(0), // 5
        K(0), // 6
        K(K_SPACE), // 7
    },
    {   //  4
        K(K_Z), // 0
        K(0), // 1
        K(0), // 2
        K(0), // 3
        K(0), // 4
        K(0), // 5
        K(0), // 6
        K(0), // 7
    },
    {   //  5
        K(K_PGUP), // 0
        K(K_W), // 1
        K(0), // 2
        K(0), // 3
        K(0), // 4
        K(0), // 5
        K(0), // 6
        K(K_CAPS), // 7
    },
    {   //  6
        K(K_PGDN), // 0
        K(K_MINUS), // 1
        K(K_E), // 2
        K(K_L), // 3
        K(0), // 4
        K(0), // 5
        K(K_M), // 6
        K(0), // 7
    },
    {   //  7
        K(K_K), // 0
        K(K_Y), // 1
        K(K_0), // 2
        K(K_R), // 3
        K(0), // 4
        K(K_SEMICOLON), // 5
        K(K_TAB), // 6
        K(K_ESC), // 7
    },
    {   //  8
        K(0), // 0
        K(K_J), // 1
        K(K_BRACE_CLOSE), // 2
        K(K_9), // 3
        K(K_T), // 4
        K(K_F7), // 5
        K(K_8), // 6
        K(K_F1), // 7
    },
    {   //  9
        K(0), // 0
        K(K_F), // 1
        K(K_H), // 2
        K(K_F2), // 3
        K(0), // 4
        K(0), // 5
        K(K_QUOTE), // 6
        K(K_B), // 7
    },
    {   // 10
        K(0), // 0
        K(K_COMMA), // 1
        K(0), // 2
        K(K_BRACE_OPEN), // 3
        K(K_S), // 4
        {K_F8, KT_SCI | SCI_BRIGHTNESS_DOWN}, // 5
        {K_F6, K_VOLUME_UP}, // 6
        K(K_2), // 7
    },
    {   // 11
        K(0), // 0
        K(K_PERIOD), // 1
        K(K_7), // 2
        K(K_D), // 3
        K(0), // 4
        K(0), // 5
        K(K_1), // 6
        K(K_P), // 7
    },
    {   // 12
        K(K_BACKSLASH), // 0
        K(0), // 1
        K(K_A), // 2
        K(K_SLASH), // 3
        K(0), // 4
        {K_F3, K_MUTE}, // 5
        K(K_I), // 6
        K(K_6), // 7
    },
    {   // 13
        K(0), // 0
        K(K_V), // 1
        K(0), // 2
        {K_F9, KT_SCI | SCI_BRIGHTNESS_UP}, // 3
        K(K_N), // 4
        K(K_O), // 5
        K(K_5), // 6
        {K_F5, K_VOLUME_DOWN}, // 7
    },
    {   // 14
        K(K_Q), // 0
        K(K_F10), // 1
        K(0), // 2
        K(K_F4), // 3
        K(0), // 4
        K(0), // 5
        K(K_DEL), // 6
        K(K_4), // 7
    },
    {   // 15
        K(K_APP), // 0
        K(K_X), // 1
        K(K_ENTER), // 2
        K(0), // 3
        K(K_U), // 4
        K(0), // 5
        K(K_3), // 6
        K(K_BKSP), // 7
    },
    {   // 16
        K(K_END), // 0
        K(K_HOME), // 1
        K(0), // 2
        K(K_UP), // 3
        K(K_DOWN), // 4
        K(K_TICK), // 5
        {K_F11, KT_SCI | SCI_AIRPLANE_MODE}, // 6
        {K_F12, KT_SCI | SCI_SUSPEND}, // 7
    },
    {   // 17
        K(K_G), // 0
        K(K_EQUALS), // 1
        K(K_C), // 2
        K(KT_FN), // 3
        K(0), // 4
        K(K_LEFT), // 5
        K(K_RIGHT), // 6
        K(0), // 7
    },
};
