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
        {0, 0}, // 0
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
        {K_F3, 0}, // 7
    },
    {   // 10
        {K_S, 0}, // 0
        {K_COMMA, 0}, // 1
        {K_L, 0}, // 2
        {K_O, 0}, // 3
        {K_9, 0}, // 4
        {K_8, 0}, // 5
        {K_F6, 0}, // 6
        {K_F5, 0}, // 7
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

// This is terrible, from http://www.vetra.com/scancodes.html
// TODO: redo by applying transform at lower level as recommended by:
// https://www.win.tue.nl/~aeb/linux/kbd/scancodes-10.html#ss10.3
uint16_t keymap_translate(uint16_t key) {
    switch (key) {
        // Function keys

        case K_F1: return 0x3B;
        case K_F2: return 0x3C;
        case K_F3: return 0x3D;
        case K_F4: return 0x3E;
        case K_F5: return 0x3F;
        case K_F6: return 0x40;
        case K_F7: return 0x41;
        case K_F8: return 0x42;
        case K_F9: return 0x43;
        case K_F10: return 0x44;
        case K_F11: return 0x57;
        case K_F12: return 0x58;

        // Number keys

        case K_0: return 0x0B;
        case K_1: return 0x02;
        case K_2: return 0x03;
        case K_3: return 0x04;
        case K_4: return 0x05;
        case K_5: return 0x06;
        case K_6: return 0x07;
        case K_7: return 0x08;
        case K_8: return 0x09;
        case K_9: return 0x0A;

        // Letter keys

        case K_A: return 0x1E;
        case K_B: return 0x30;
        case K_C: return 0x2E;
        case K_D: return 0x20;
        case K_E: return 0x12;
        case K_F: return 0x21;
        case K_G: return 0x22;
        case K_H: return 0x23;
        case K_I: return 0x17;
        case K_J: return 0x24;
        case K_K: return 0x25;
        case K_L: return 0x26;
        case K_M: return 0x32;
        case K_N: return 0x31;
        case K_O: return 0x18;
        case K_P: return 0x19;
        case K_Q: return 0x10;
        case K_R: return 0x13;
        case K_S: return 0x1F;
        case K_T: return 0x14;
        case K_U: return 0x16;
        case K_V: return 0x2F;
        case K_W: return 0x11;
        case K_X: return 0x2D;
        case K_Y: return 0x15;
        case K_Z: return 0x2C;

        // Special keys

        case K_ESC: return 0x01;

        case K_TICK: return 0x29;
        case K_MINUS: return 0x0C;
        case K_EQUALS: return 0x0D;
        case K_BKSP: return 0x0E;

        case K_TAB: return 0x0F;
        case K_BRACE_OPEN: return 0x1A;
        case K_BRACE_CLOSE: return 0x1B;
        case K_BACKSLASH: return 0x2B;

        case K_CAPS: return 0x3A;
        case K_SEMICOLON: return 0x27;
        case K_QUOTE: return 0x28;
        case K_ENTER: return 0x1C;

        case K_LEFT_SHIFT: return 0x2A;
        case K_COMMA: return 0x33;
        case K_PERIOD: return 0x34;
        case K_SLASH: return 0x35;
        case K_RIGHT_SHIFT: return 0x36;

        case K_LEFT_CTRL: return 0x1D;
        case K_LEFT_SUPER: return (K_E0 | 0x5B);
        case K_LEFT_ALT: return 0x38;
        case K_SPACE: return 0x39;
        case K_RIGHT_ALT: return (K_E0 | 0x38);
        case K_RIGHT_SUPER: return (K_E0 | 0x5C);
        case K_APP: return (K_E0 | 0x5D);
        case K_RIGHT_CTRL: return (K_E0 | 0x1D);

        case K_INSERT: return (K_E0 | 0x52);
        case K_DEL: return (K_E0 | 0x53);
        case K_HOME: return (K_E0 | 0x47);
        case K_END: return (K_E0 | 0x4F);
        case K_PGUP: return (K_E0 | 0x49);
        case K_PGDN: return (K_E0 | 0x51);

        case K_UP: return (K_E0 | 0x48);
        case K_LEFT: return (K_E0 | 0x4B);
        case K_DOWN: return (K_E0 | 0x50);
        case K_RIGHT: return (K_E0 | 0x4D);

        default:
            WARN("keymap_translate: unknown %02X\n", key);
            return 0;
    }
}
