#ifndef _EC_PARALLEL_H
#define _EC_PARALLEL_H

#include <stdint.h>

void parport_init(void);
int parport_write(uint8_t * data, int length);

#endif // _EC_PARALLEL_H
