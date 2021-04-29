// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_DEVICE_H
#define _BOARD_DEVICE_H

#include <stdint.h>

#include <arch/gpio.h>
#include <arch/timer.h>
#include <board/debounce.h>
#include <board/tach.h>
#include <board/transition.h>

struct Device;

typedef void (*DeviceCallback)(struct Device * device, uint64_t time, void * data);

enum InputState {
    INPUT_STATE_UNKNOWN = 0,
    INPUT_STATE_LOW = 1,
    INPUT_STATE_FALLING = 2,
    INPUT_STATE_RISING = 3,
    INPUT_STATE_HIGH = 4,
};

struct Device {
    // Name of device
    char * name;
    // Time of last command
    uint64_t command_time;
    // Input pin
    struct Gpio * input;
    // Debounce input pin to prevent erroneous input
    struct Debounce input_db;
    // Last value of input pin to detect rising and falling
    uint8_t input_last;
    // State of input pin: low, falling, rising, or high
    enum InputState input_state;
    // Count of input pin rises
    struct Tach input_tach;
    // Output pin
    struct Gpio * output;
    // Timer used to drive output pin
    struct Timer * output_timer;
    // Channel of timer used to drive output pin
    enum TimerChannel output_channel;
    // Duty cycle of output pin, from 0 to 10000
    uint16_t output_duty;
    // Smoothly transition output pin between duty cycles
    struct Transition output_trans;
    // Run a callback every time the device is updated
    DeviceCallback callback;
    // Provide custom data to the callback
    void * callback_data;
};

struct Device device_new(char * name, struct Gpio * input, struct Gpio * output, struct Timer * output_timer, enum TimerChannel output_channel, uint16_t output_duty, uint64_t transition_time);

void device_init(struct Device * device);

void device_set_callback(struct Device * device, DeviceCallback callback, void * callback_data);

void device_step(struct Device * device, uint64_t time);

void device_destroy(struct Device * device);

#endif // _BOARD_DEVICE_H
