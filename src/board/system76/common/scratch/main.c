// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2019 System76, Inc.

#include <board/smfi.h>

// Main program while running in scratch ROM
void main(void) {
    for (;;) {
        smfi_event();
    }
}
