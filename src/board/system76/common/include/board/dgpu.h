#ifndef _BOARD_DGPU_H
#define _BOARD_DGPU_H

#ifndef HAVE_DGPU
    #define HAVE_DGPU 0
#endif

void dgpu_init(void);
void dgpu_event(void);

#endif // _BOARD_DGPU_H
