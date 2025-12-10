// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2020 System76, Inc.

#include <app/ecpm.h>
#include <common/macro.h>

void ecpm_init(void) {
    // Clock gate EGPC, CIR, and SWUC
    CGCTRL2 |= BIT(6) | BIT(5) | BIT(4);
    // Clock gate UART, SSPI, and DBGR
    CGCTRL3 |= BIT(2) | BIT(1) | BIT(0);
    // Clock gate CEC
    CGCTRL4 |= BIT(0);
}
