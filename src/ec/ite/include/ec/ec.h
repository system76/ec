// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_EC_H
#define _EC_EC_H

// Last reset source
enum EcResetSource {
    RESET_SOURCE_NORMAL,
    RESET_SOURCE_WDT,
    RESET_SOURCE_PWRSW_TIMEOUT,
};

void ec_init(void);
void ec_read_post_codes(void);
enum EcResetSource ec_reset_source(void);

#endif // _EC_EC_H
