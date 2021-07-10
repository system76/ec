// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_TRANSITION_H
#define _BOARD_TRANSITION_H

#include <stdint.h>

struct Transition {
    double speed;
    uint64_t time;
    double start;
    double target;
    double value;
};

struct Transition transition_new(double speed);

void transition_set(struct Transition * transition, uint64_t time, double target);
double transition_step(struct Transition * transition, uint64_t time);

#endif // _BOARD_TRANSITION_H
