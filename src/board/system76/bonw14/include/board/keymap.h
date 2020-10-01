// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_KEYMAP_H
#define _BOARD_KEYMAP_H

#include <common/keymap.h>

#define ___ 0

// Conversion of physical layout to keyboard matrix
#define LAYOUT( \
    K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, K0E, K0F, K0G, K0H, K0I, K0J, \
    K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, K1E, K1F, K1G, K1H, \
    K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, K2D, K2E, K2F, K2G, K2H, \
    K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C, K3D, K3E, K3F, \
    K40, K41, K42, K43, K44, K45, K46, K47, K48, K49, K4A, K4B, K4C, K4D, K4E, K4F, K4G, \
    K50, K51, K52, K53, K54, K55, K56, K57, K58, K59, K5A, K5B, K5C \
) { /*  0,   1,   2,   3,   4,   5,   6,   7 */ \
    { K10, K31, K44, K27, K29, K4A, K1E, ___ }, /* 0 */ \
    { K50, K02, K15, K36, K48, K3B, K2E, ___ }, /* 1 */ \
    { K30, K12, K25, K18, K2A, ___, K3D, K55 }, /* 2 */ \
    { K40, K22, K35, K46, K1B, ___, K4D, K56 }, /* 3 */ \
    { K51, K13, K06, K37, K3A, K4B, K0H, K57 }, /* 4 */ \
    { K20, K42, K16, K28, K49, ___, K1F, K58 }, /* 5 */ \
    { K00, K32, K07, K09, K0C, K0E, K1G, K59 }, /* 6 */ \
    { ___, K03, K45, K38, K1C, K1D, K2F, K5A }, /* 7 */ \
    { K21, K33, K26, K47, K2B, K3C, K4E, K5B }, /* 8 */ \
    { K41, K43, K17, K19, K0D, K2D, K3E, K5C }, /* 9 */ \
    { K11, K23, K08, K39, K2C, K4C, K2G, K0J }, /* 10 */ \
    { K52, K04, ___, K0A, ___, K0F, K3F, K1H }, /* 11 */ \
    { K01, K14, ___, K1A, ___, K0G, K4F, K2H }, /* 12 */ \
    { K53, K24, ___, K0B, ___, ___, K0I, ___ }, /* 13 */ \
    { ___, K34, ___, ___, ___, ___, ___, K4G }, /* 14 */ \
    { K54, K05, ___, ___, ___, ___, ___, ___ }, /* 15 */ \
    { ___, ___, ___, ___, ___, ___, ___, ___ }, /* 16 */ \
    { ___, ___, ___, ___, ___, ___, ___, ___ }  /* 17 */ \
}

// Keymap output pins
#define KM_OUT 18
// Keymap input pins
#define KM_IN 8
// Keymap layers (normal, Fn)
#define KM_LAY 2
// Keyboard has n-key rollover
#define KM_NKEY 1

// Keymap
extern uint16_t __xdata KEYMAP[KM_LAY][KM_OUT][KM_IN];

// Position of physical Esc key in the matrix
#define MATRIX_ESC_INPUT    0
#define MATRIX_ESC_OUTPUT   6

// Position of physical Fn key in the matrix
#define MATRIX_FN_INPUT     0
#define MATRIX_FN_OUTPUT    4

#endif // _BOARD_KEYMAP_H
