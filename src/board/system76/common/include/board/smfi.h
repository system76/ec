// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_SMFI_H
#define _BOARD_SMFI_H

void smfi_init(void);
void smfi_watchdog(void);
void smfi_event(void);
void smfi_debug(unsigned char byte);

#endif // _BOARD_SMFI_H
