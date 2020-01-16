#ifndef _EC_I2C_H
#define _EC_I2C_H

#include <common/i2c.h>

extern struct I2C __code I2C_4;

void i2c_reset(struct I2C * i2c, bool kill);

#endif // _EC_I2C_H