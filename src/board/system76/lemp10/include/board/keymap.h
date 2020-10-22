// SPDX-License-Identifier: GPL-3.0-only

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

// Conversion of physical layout to keyboard matrix
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

// Position of physical Esc key in the matrix
#define MATRIX_ESC_INPUT    7
#define MATRIX_ESC_OUTPUT   7

// Position of physical Fn key in the matrix
#define MATRIX_FN_INPUT     0
#define MATRIX_FN_OUTPUT    6

#endif // _BOARD_KEYMAP_H
