#include <common/macro.h>

static const char __code BOARD[] =
    "76EC_BOARD="
    xstr(__BOARD__);

static const char __code VERSION[] =
    "76EC_VERSION="
    xstr(__VERSION__);

const char * board() {
    return &BOARD[11];
}

const char * version() {
    return &VERSION[13];
}
