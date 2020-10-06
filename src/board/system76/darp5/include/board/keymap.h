// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_KEYMAP_H
#define _BOARD_KEYMAP_H

// Keymap layers (normal, Fn)
#define KM_LAY 2
// Keymap output pins
#define KM_OUT 18
// Keymap input pins
#define KM_IN 8

// common/keymap.h requires KM_LAY, KM_OUT, and KM_IN definitions
#include <common/keymap.h>

// International keys
#ifndef KI1
    #define KI1 K_INT_1
#endif
#ifndef KI2
    #define KI2 K_INT_2
#endif

// Conversion of physical layout to keyboard matrix
#define LAYOUT( \
    K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, K0E, K0F, K0G, K0H, K0I, K0J, \
    K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, K1E, K1F, K1G, K1H, \
    K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, K2D, K2E, K2F, K2G, K2H, \
    K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C, K3D, K3E, K3F, \
    K40, K41, K42, K43, K44, K45, K46, K47, K48, K49, K4A, K4B, K4C, K4D, K4E, K4F, K4G, \
    K50, K51, K52, K53, K54, K55, K56, K57, K58, K59, K5A, K5B, K5C \
) { \
    { ___, ___, ___, ___, ___, ___, K50, K57 }, \
    { ___, ___, ___, ___, ___, ___, K53, K55 }, \
    { ___, ___, ___, ___, ___, ___, K40, K4B }, \
    { K52, ___, ___, ___, ___, ___, ___, K54 }, \
    { K41, K1H, ___, K2F, K4E, ___, ___, ___ }, \
    { K0I, K22, ___, ___, K1E, K3D, K3F, K30 }, \
    { K0J, K1B, K23, K39, ___, ___, K47, ___ }, \
    { K38, K26, K1A, K24, ___, K3A, K20, K00 }, \
    { K5B, K37, K2C, K19, K25, K07, K18, K01 }, \
    { K5C, K34, K36, K02, ___, K4G, K3B, K45 }, \
    { K1F, K48, ___, K2B, K32, K08, K06, K12 }, \
    { K1G, K49, K17, K33, ___, ___, K11, K2A }, \
    { K2D, KI1, K31, K4A, KI2, K03, K28, K16 }, \
    { ___, K44, K0D, K09, K46, K29, K15, K05 }, \
    { K21, K0A, K2E, K04, K3E, K0E, K0F, K14 }, \
    { K56, K42, K3C, K2H, K27, K2G, K13, K1D }, \
    { K0H, K0G, K43, K4C, K59, K10, K0B, K0C }, \
    { K35, K1C, K4F, K51, K4D, K58, K5A, ___ } \
}

// Position of physical Esc key in the matrix
#define MATRIX_ESC_INPUT    7
#define MATRIX_ESC_OUTPUT   7

// Position of physical Fn key in the matrix
#define MATRIX_FN_INPUT     3
#define MATRIX_FN_OUTPUT    17

#endif // _BOARD_KEYMAP_H
