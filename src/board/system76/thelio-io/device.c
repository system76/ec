// SPDX-License-Identifier: GPL-3.0-only

#include <stdlib.h>

#include <board/device.h>

struct Device device_new(char * name, struct Gpio * input, struct Gpio * output, struct Timer * output_timer, enum TimerChannel output_channel, uint16_t output_duty, uint64_t transition_time) {
    struct Device device = {
        .name = name,
        .command_time = 0,
        .input = input,
        .input_db = debounce_new(1000ULL),
        .input_last = 0,
        .input_state = INPUT_STATE_UNKNOWN,
        .input_tach = tach_new(1000000ULL),
        .output = output,
        .output_timer = output_timer,
        .output_channel = output_channel,
        .output_duty = output_duty,
        .output_trans = transition_new(10000.0 / (double)transition_time),
        .callback = NULL,
        .callback_data = NULL,
    };

    return device;
}

void device_init(struct Device * device) {
    gpio_set_dir(device->input, 0);
    gpio_set(device->input, 1);

    gpio_set_dir(device->output, 1);
    gpio_set(device->output, 0);

    device->input_last = gpio_get(device->input);
    device->input_db.value = device->input_last;

    timer_set_channel_mode(device->output_timer, device->output_channel, TIMER_CHANNEL_MODE_CLEAR);
    timer_set_channel_duty(device->output_timer, device->output_channel, device->output_duty);

    device->output_trans.value = (double)device->output_duty;
}

void device_set_callback(struct Device * device, DeviceCallback callback, void * callback_data) {
    device->callback = callback;
    device->callback_data = callback_data;
}

// Update device
void device_step(struct Device * device, uint64_t time) {
    // Run input items
    uint8_t input = debounce_step(&device->input_db, time, gpio_get(device->input));
    if (input) {
        if (device->input_last) {
            device->input_state = INPUT_STATE_HIGH;
        } else {
            device->input_state = INPUT_STATE_RISING;
        }
    } else {
        if (device->input_last) {
            device->input_state = INPUT_STATE_FALLING;
        } else {
            device->input_state = INPUT_STATE_LOW;
        }
    }
    device->input_last = input;

    uint16_t tach_value = 0;
    switch (device->input_state) {
        case INPUT_STATE_RISING:
            tach_value = 1;
            break;
        default:
            break;
    }
    tach_step(&device->input_tach, time, tach_value);

    // Run callback
    if (device->callback) {
        device->callback(device, time, device->callback_data);
    }

    // Run output items
    uint16_t duty = (uint16_t)transition_step(&device->output_trans, time);
    if (duty != device->output_duty) {
        //TODO: Handle failure to set
        if (timer_set_channel_duty(device->output_timer, device->output_channel, duty) == 0) {
            device->output_duty = duty;
        }
    }
}

void device_destroy(struct Device * device) {
    timer_set_channel_duty(device->output_timer, device->output_channel, 0);
    timer_set_channel_mode(device->output_timer, device->output_channel, TIMER_CHANNEL_MODE_NORMAL);

    gpio_set_dir(device->output, 0);
    gpio_set(device->output, 0);

    gpio_set_dir(device->input, 0);
    gpio_set(device->input, 0);
}
