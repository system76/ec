// SPDX-License-Identifier: GPL-3.0-only

#include <board/ecpm.h>

void ecpm_init(void) {
    // Clock gate EGPC, CIR, and SWUC
    CGCTRL2 |= (1 << 6) | (1 << 5) | (1 << 4);
    // Clock gate UART, SSPI, and DBGR
    CGCTRL3 |= (1 << 2) | (1 << 1) | (1 << 0);
    // Clock gate CEC
    CGCTRL4 |= (1 << 0);
}
