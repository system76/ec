#ifndef _BOARD_DGPU_H
#define _BOARD_DGPU_H

#include <stdint.h>

#ifndef HAVE_DGPU
    #define HAVE_DGPU 0
#endif

#if HAVE_DGPU
    extern int16_t dgpu_temp;
    extern uint8_t dgpu_duty;
#endif // HAVE_DGPU

void dgpu_init(void);
void dgpu_event(void);

#endif // _BOARD_DGPU_H
