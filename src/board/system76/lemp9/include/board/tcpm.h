#ifndef _BOARD_TCPM_H
#define _BOARD_TCPM_H

#include <ec/smbus.h>

void tcpm_init(void);
int tcpm_read(uint8_t address, uint8_t command, uint16_t * data);
int tcpm_write(uint8_t address, uint8_t command, uint16_t data);
void tcpm_event(void);

#endif // _BOARD_TCPM_H
