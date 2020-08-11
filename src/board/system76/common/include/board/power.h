#ifndef _BOARD_POWER_H
#define _BOARD_POWER_H

enum PowerState {
    POWER_STATE_DEFAULT,
    POWER_STATE_DS5,
    POWER_STATE_S5,
    POWER_STATE_DS3,
    POWER_STATE_S3,
    POWER_STATE_S0,
};

extern enum PowerState power_state;

void power_init(void);
void power_event(void);

#endif // _BOARD_POWER_H
