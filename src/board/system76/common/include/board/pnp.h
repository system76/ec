#ifndef _BOARD_PNP_H
#define _BOARD_PNP_H

#include <stdint.h>

uint8_t pnp_read(uint8_t reg);
void pnp_write(uint8_t reg, uint8_t data);
void pnp_enable(void);

#endif // _BOARD_PNP_H
