// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_I2C_H
#define _EC_I2C_H

#include <common/i2c.h>

extern struct I2C __code I2C_0;
extern struct I2C __code I2C_1;
#if CONFIG_EC_ITE_IT5570E || CONFIG_EC_ITE_IT5571E
extern struct I2C __code I2C_4;
#endif

void i2c_reset(struct I2C *i2c, bool kill);

#endif // _EC_I2C_H
