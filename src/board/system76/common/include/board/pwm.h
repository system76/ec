// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_PWM_H
#define _BOARD_PWM_H

#include <ec/pwm.h>

// NOTE: These are used instead of the functions directly for ACPI to prevent
// double reads of the tachometer values.
extern int16_t pwm_tach0_rpm;
extern int16_t pwm_tach1_rpm;

void pwm_init(void);
int16_t pwm_get_tach0_rpm(void);
int16_t pwm_get_tach1_rpm(void);

#endif // _BOARD_PWM_H
