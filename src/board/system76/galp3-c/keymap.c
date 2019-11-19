#include <board/keymap.h>
#include <common/debug.h>

uint16_t __code KEYMAP[KM_OUT][KM_IN][KM_LAY] = {
    {   //  0
        {0, 0}, // 0
        {0, 0}, // 1
        {0, 0}, // 2
        {0, 0}, // 3
        {0, 0}, // 4
        {0, 0}, // 5
        {K_LEFT_SUPER, 0}, // 6
        {0, 0}, // 7
    },
    {   //  1
        {0, 0}, // 0
        {0, 0}, // 1
        {0, 0}, // 2
        {0, 0}, // 3
        {0, 0}, // 4
        {0, 0}, // 5
        {K_LEFT_CTRL, 0}, // 6
        {K_RIGHT_CTRL, 0}, // 7
    },
    {   //  2
        {0, 0}, // 0
        {0, 0}, // 1
        {0, 0}, // 2
        {0, 0}, // 3
        {0, 0}, // 4
        {0, 0}, // 5
        {K_LEFT_ALT, 0}, // 6
        {K_RIGHT_ALT, 0}, // 7
    },
    {   //  3
        {0, 0}, // 0
        {0, 0}, // 1
        {0, 0}, // 2
        {0, 0}, // 3
        {0, 0}, // 4
        {0, 0}, // 5
        {K_LEFT_SHIFT, 0}, // 6
        {K_RIGHT_SHIFT, 0}, // 7
    },
    {   //  4
        {K_X, 0}, // 0
        {K_Z, 0}, // 1
        {K_CAPS, 0}, // 2
        {K_A, 0}, // 3
        {K_TAB, 0}, // 4
        {0, 0}, // 5
        {K_SPACE, 0}, // 6
        {K_Q, 0}, // 7
    },
    {   //  5
        {K_V, 0}, // 0
        {K_C, 0}, // 1
        {0, 0}, // 2
        {K_D, 0}, // 3
        {0, 0}, // 4
        {0, 0}, // 5
        {0, 0}, // 6
        {K_E, 0}, // 7
    },
    {   //  6
        {K_FN, K_FN}, // 0
        {K_B, 0}, // 1
        {K_F, 0}, // 2
        {K_G, 0}, // 3
        {K_R, 0}, // 4
        {K_T, 0}, // 5
        {0, 0}, // 6
        {0, 0}, // 7
    },
    {   //  7
        {0, 0}, // 0
        {K_N, 0}, // 1
        {K_H, 0}, // 2
        {K_Y, 0}, // 3
        {K_2, 0}, // 4
        {K_1, 0}, // 5
        {K_TICK, 0}, // 6
        {K_ESC, 0}, // 7
    },
    {   //  8
        {K_I, 0}, // 0
        {K_M, 0}, // 1
        {K_U, 0}, // 2
        {K_5, 0}, // 3
        {K_4, 0}, // 4
        {K_3, 0}, // 5
        {K_F2, 0}, // 6
        {K_F1, 0}, // 7
    },
    {   //  9
        {K_W, 0}, // 0
        {K_J, 0}, // 1
        {K_K, 0}, // 2
        {0, 0}, // 3
        {K_7, 0}, // 4
        {K_6, 0}, // 5
        {K_F4, 0}, // 6
        {K_F3, K_MUTE}, // 7
    },
    {   // 10
        {K_S, 0}, // 0
        {K_COMMA, 0}, // 1
        {K_L, 0}, // 2
        {K_O, 0}, // 3
        {K_9, 0}, // 4
        {K_8, 0}, // 5
        {K_F6, K_VOLUME_UP}, // 6
        {K_F5, K_VOLUME_DOWN}, // 7
    },
    {   // 11
        {0, 0}, // 0
        {K_UP, 0}, // 1
        {K_PERIOD, 0}, // 2
        {K_SEMICOLON, 0}, // 3
        {K_P, 0}, // 4
        {K_0, 0}, // 5
        {K_F8, 0}, // 6
        {K_F7, 0}, // 7
    },
    {   // 12
        {0, 0}, // 0
        {0, 0}, // 1
        {K_SLASH, 0}, // 2
        {K_QUOTE, 0}, // 3
        {K_BRACE_OPEN, 0}, // 4
        {K_MINUS, 0}, // 5
        {K_F10, 0}, // 6
        {K_F9, 0}, // 7
    },
    {   // 13
        {0, 0}, // 0
        {0, 0}, // 1
        {0 /* TODO: PrtSc/SysRq */, 0}, // 2
        {K_BRACE_CLOSE, 0}, // 3
        {K_EQUALS, 0}, // 4
        {K_RIGHT, 0}, // 5
        {0 /* TODO: Pause/Break */, 0}, // 6
        {K_F11, 0}, // 7
    },
    {   // 14
        {0, 0}, // 0
        {K_BACKSLASH, 0}, // 1
        {K_LEFT, 0}, // 2
        {K_BKSP, 0}, // 3
        {K_PGUP, 0}, // 4
        {K_INSERT, 0}, // 5
        {K_HOME, 0}, // 6
        {K_DEL, 0}, // 7
    },
    {   // 15
        {K_APP, 0}, // 0
        {0, 0}, // 1
        {K_ENTER, 0}, // 2
        {0, 0}, // 3
        {K_DOWN, 0}, // 4
        {K_F12, 0}, // 5
        {K_END, 0}, // 6
        {K_PGDN, 0}, // 7
    },
};

uint16_t keymap(int output, int input, int layer) {
    if (output < KM_OUT && input < KM_IN && layer < KM_LAY) {
        return KEYMAP[output][input][layer];
    } else {
        return 0;
    }
}

// https://www.win.tue.nl/~aeb/linux/kbd/scancodes-10.html#ss10.3
static uint8_t __code lookup[256] = {
0xff, 0x43, 0x41, 0x3f, 0x3d, 0x3b, 0x3c, 0x58, 0x64, 0x44, 0x42, 0x40, 0x3e, 0x0f, 0x29, 0x59,
0x65, 0x38, 0x2a, 0x70, 0x1d, 0x10, 0x02, 0x5a, 0x66, 0x71, 0x2c, 0x1f, 0x1e, 0x11, 0x03, 0x5b,
0x67, 0x2e, 0x2d, 0x20, 0x12, 0x05, 0x04, 0x5c, 0x68, 0x39, 0x2f, 0x21, 0x14, 0x13, 0x06, 0x5d,
0x69, 0x31, 0x30, 0x23, 0x22, 0x15, 0x07, 0x5e, 0x6a, 0x72, 0x32, 0x24, 0x16, 0x08, 0x09, 0x5f,
0x6b, 0x33, 0x25, 0x17, 0x18, 0x0b, 0x0a, 0x60, 0x6c, 0x34, 0x35, 0x26, 0x27, 0x19, 0x0c, 0x61,
0x6d, 0x73, 0x28, 0x74, 0x1a, 0x0d, 0x62, 0x6e, 0x3a, 0x36, 0x1c, 0x1b, 0x75, 0x2b, 0x63, 0x76,
0x55, 0x56, 0x77, 0x78, 0x79, 0x7a, 0x0e, 0x7b, 0x7c, 0x4f, 0x7d, 0x4b, 0x47, 0x7e, 0x7f, 0x6f,
0x52, 0x53, 0x50, 0x4c, 0x4d, 0x48, 0x01, 0x45, 0x57, 0x4e, 0x51, 0x4a, 0x37, 0x49, 0x46, 0x54,
0x80, 0x81, 0x82, 0x41, 0x54, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
};

uint16_t keymap_translate(uint16_t key) {
    return (key & 0xFF00) | lookup[(key & 0xFF)];
}
