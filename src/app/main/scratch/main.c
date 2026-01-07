// SPDX-License-Identifier: GPL-3.0-only

#include <app/smfi.h>

// Main program while running in scratch ROM
void main(void) {
    for (;;) {
        smfi_event();
    }
}
