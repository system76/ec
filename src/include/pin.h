#ifndef _PIN_H_
#define _PIN_H_

#include <stdbool.h>

#include "gpio.h"

struct Pin {
    __xdata volatile unsigned char * data;
    __xdata volatile unsigned char * mirror;
    __xdata volatile unsigned char * control;
    unsigned char value;
};

#define PIN(BLOCK, NUMBER) { \
    .data = &GPDR ## BLOCK, \
    .mirror = &GPDMR ## BLOCK, \
    .control = &GPCR ## BLOCK ## NUMBER, \
    .value = (1 << NUMBER), \
}

bool pin_get(struct Pin * pin);
void pin_set(struct Pin * pin, bool value);

#endif // _PIN_H_
