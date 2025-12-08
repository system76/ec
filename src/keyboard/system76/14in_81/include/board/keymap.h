// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2024 System76, Inc.

#ifndef _BOARD_KEYMAP_H
#define _BOARD_KEYMAP_H

// Keymap layers (normal, Fn)
#define KM_LAY 2
// Keymap output pins
#define KM_OUT 16
// Keymap input pins
#define KM_IN 8

// common/keymap.h requires KM_LAY, KM_OUT, and KM_IN definitions
#include <common/keymap.h>

// Conversion of physical ANSI layout to keyboard matrix
// uncrustify:off
#define LAYOUT( \
    K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, K0E, K0F, K0G, \
    K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, \
    K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, K2D, \
    K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C, \
    K40, K41, K42, K43, K44, K45, K46, K47, K48, K49, K4A, K4B, K4C, \
    K50, K51, K52, K53, K54, K55, K56, K57, K58, K59 \
) { \
      /*  0,   1,   2,   3,   4,   5,   6,   7 */ \
/*0*/ { ___, ___, ___, K54, ___, ___, ___, K52 }, \
/*1*/ { ___, ___, ___, ___, ___, K56, K50, ___ }, \
/*2*/ { ___, ___, ___, K55, K53, ___, ___, ___ }, \
/*3*/ { ___, K4A, K40, ___, ___, ___, ___, ___ }, \
/*4*/ { ___, K41, K30, K33, K20, K43, ___, K21 }, \
/*5*/ { ___, K42, ___, K31, K23, ___, ___, K11 }, \
/*6*/ { K51, K45, K27, K35, K24, ___, K44, ___ }, \
/*7*/ { K32, K46, K36, ___, K26, K12, K10, K00 }, \
/*8*/ { K28, K47, K34, K25, K15, K13, K02, K03 }, \
/*9*/ { K22, K17, K38, ___, K14, K04, K16, K01 }, \
/*10*/{ K1A, K48, K39, K29, K19, K18, K06, K05 }, \
/*11*/{ K4B, K37, K49, K3A, K2A, ___, K08, K07 }, \
/*12*/{ ___, ___, K4C, K3B, K2B, K1B, K0A, K09 }, \
/*13*/{ ___, ___, K0D, K2C, K1C, K0B, K0E, K59 }, \
/*14*/{ ___, K2D, K57, K1D, ___, K0F, ___, K0G }, \
/*15*/{ ___, ___, K3C, ___, K0C, ___, ___, K58 } \
}
// uncrustify:on

// Position of physical Esc key in the matrix
#define MATRIX_ESC_INPUT 7
#define MATRIX_ESC_OUTPUT 7

// Position of physical Fn key in the matrix
#define MATRIX_FN_INPUT 0
#define MATRIX_FN_OUTPUT 6

static inline bool keymap_is_f_key(uint8_t row, uint8_t col) {
    switch (row) {
    case 8:
        // F2, F3
        return col == 6 || col == 7;
    case 9:
        // F4, F1
        return col == 5 || col == 7;
    case 10:
        // F6, F5
        return col == 6 || col == 7;
    case 11:
        // F8, F7
        return col == 6 || col == 7;
    case 12:
        // F10, F9
        return col == 6 || col == 7;
    case 13:
        // F11
        return col == 5;
    case 15:
        // F12
        return col == 4;
    default:
        return false;
    }
}

#endif // _BOARD_KEYMAP_H
