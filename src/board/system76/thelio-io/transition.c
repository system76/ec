// SPDX-License-Identifier: GPL-3.0-only

#include <board/transition.h>

struct Transition transition_new(double speed) {
    struct Transition transition = {
        .speed = speed,
        .time = 0,
        .start = 0.0,
        .target = 0.0,
        .value = 0.0,
    };

    return transition;
}

void transition_set(struct Transition * transition, uint64_t time, double target) {
    transition->time = time;
    transition->start = transition->value;
    transition->target = target;
}

double transition_step(struct Transition * transition, uint64_t time) {
    if (transition->time) {
        double interval = (double)(time - transition->time);
        double delta = interval * transition->speed;

        if (transition->start > transition->target) {
            transition->value = transition->start - delta;
            if (transition->value < transition->target) {
                transition->value = transition->target;
                transition->time = 0;
            }
        } else if (transition->start < transition->target) {
            transition->value = transition->start + delta;
            if (transition->value > transition->target) {
                transition->value = transition->target;
                transition->time = 0;
            }
        } else {
            transition->time = 0;
        }
    }

    return transition->value;
}
