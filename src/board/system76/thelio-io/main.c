// SPDX-License-Identifier: GPL-3.0-only

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arch/arch.h>
#include <arch/gpio.h>
#include <arch/time.h>
#include <arch/timer.h>
#include <arch/uart.h>
#include <board/config.h>
#include <board/cpu.h>
#include <board/device.h>
#include <board/tach.h>
#include <common/macro.h>
#include <common/version.h>
#include <util/delay.h>
#include "usb/USBtoSerial.h"

static void timer_fast_pwm(struct Timer * timer, uint32_t frequency) {
    timer_init(timer);
    timer_set_mode(timer, TIMER_MODE_FAST);
    timer_set_frequency(timer, frequency);
}

static int usb_put(char data, FILE * stream) {
    if (USBtoSerial_Write((uint8_t)data)) {
        return _FDEV_ERR;
    } else {
        return 0;
    }
}

static FILE usb_file = FDEV_SETUP_STREAM(usb_put, NULL, _FDEV_SETUP_WRITE);

enum PowerBtnState {
    POWERBTN_OFF,
    POWERBTN_ON,
    POWERBTN_SUSPEND,
    POWERBTN_PRESS,
};

struct Thelio {
    // Config
    struct Config config;
    // Variables
    uint8_t bootloader;
    unsigned char motherled_off;
    enum PowerBtnState powerbtn_state;
    uint16_t suspend_state;
    // Timers
    struct Timer * timer_1;
    struct Timer * timer_3;
    struct Timer * timer_4;
    // Pins
    struct Gpio * motherled;
    struct Gpio * motherbtn;
    struct Gpio * cpufanmux;
    // Devices
    struct Device powerbtn;
    struct Device cpufan;
    struct Device intakefan;
    struct Device exhaustfan;
    struct Device * devices[4];
};

static void thelio_powerbtn_callback(struct Device * device, uint64_t time, void * data) {
    struct Thelio * thelio = (struct Thelio *)data;

    uint8_t motherbtn;
    enum PowerBtnState powerbtn_state;
    switch (device->input_state) {
        case INPUT_STATE_FALLING:
        case INPUT_STATE_LOW:
            motherbtn = 0;
            powerbtn_state = POWERBTN_PRESS;
            break;
        default:
            motherbtn = 1;
            if (thelio->suspend_state) {
                powerbtn_state = POWERBTN_SUSPEND;
            } else if (gpio_get(thelio->motherled) == thelio->motherled_off) {
                powerbtn_state = POWERBTN_OFF;
            } else {
                powerbtn_state = POWERBTN_ON;
            }
            break;
    }
    gpio_set(thelio->motherbtn, motherbtn);

    if (powerbtn_state != thelio->powerbtn_state) {
        thelio->powerbtn_state = powerbtn_state;

        uint32_t transition = thelio->config.led_transition;
        uint16_t duty = 0;
        switch (powerbtn_state) {
            case POWERBTN_OFF:
                duty = thelio->config.powerbtn_off;
                break;
            case POWERBTN_SUSPEND:
                transition = thelio->config.suspend_transition;
                duty = thelio->config.powerbtn_suspend;
                break;
            case POWERBTN_ON:
                duty = thelio->config.powerbtn_on;
                break;
            case POWERBTN_PRESS:
                duty = thelio->config.powerbtn_pressed;
                break;
        }

        device->output_trans.speed = 10000.0 / (double)transition;
        transition_set(&device->output_trans, time, (double)duty);
    }

    if (powerbtn_state == POWERBTN_SUSPEND && device->output_trans.time == 0) {
        if (device->output_duty == thelio->config.powerbtn_suspend) {
            transition_set(&device->output_trans, time, (double)thelio->config.powerbtn_off);
        } else {
            transition_set(&device->output_trans, time, (double)thelio->config.powerbtn_suspend);
        }
    }
}


static void thelio_cpufan_callback(struct Device * device, uint64_t time, void * data) {
    struct Thelio * thelio = (struct Thelio *)data;

    uint32_t interval = (uint32_t)(time - device->command_time);
    if (interval > thelio->config.cpufan_timeout) {
        // Use motherboard CPU fan
        gpio_set(thelio->cpufanmux, 0);
    } else if(device->command_time > 0) {
        // Use software fan control
        gpio_set(thelio->cpufanmux, 1);
    }
}

uint8_t thelio_new(struct Thelio * thelio) {
    thelio->config = config_new();
    config_load(&thelio->config);

    thelio->bootloader = 0;
    thelio->powerbtn_state = POWERBTN_OFF;
    thelio->suspend_state = 0;

    // Allocate timers
    #define GETTIMER(N) \
        thelio->timer_ ## N = timer_from_name(#N); \
        if (!thelio->timer_ ## N) { \
            printf("Failed to find timer %s\n", #N); \
            return 1; \
        }

    // Timer for CPU and intake fans
    GETTIMER(1);
    // Timer for exhaust fan
    GETTIMER(3);
    // Timer for power button
    GETTIMER(4);

    // Allocate pins
    #define GETPIN(B, N) \
        static struct Gpio pin_P ## B ## N = GPIO(B, N);

    {
        GETPIN(F, 0); // ADC0
        GETPIN(B, 4); // PB4
        GETPIN(E, 6); // PE6
        thelio->motherled = &pin_PF0;
        thelio->motherbtn = &pin_PB4;
        thelio->cpufanmux = &pin_PE6;
    }

    int device_i = 0;

    {
        GETPIN(F, 1); // ADC1
        GETPIN(C, 7); // OC4A
        thelio->powerbtn = device_new(
            "POWB",
            &pin_PF1, &pin_PC7,
            thelio->timer_4, TIMER_CHANNEL_A,
            thelio->config.powerbtn_off, (double)thelio->config.led_transition
        );
        device_set_callback(&thelio->powerbtn, thelio_powerbtn_callback, thelio);
        thelio->devices[device_i++] = &thelio->powerbtn;
    }

    {
        GETPIN(F, 4); // ADC4
        GETPIN(B, 5); // OC1A
        thelio->cpufan = device_new(
            "CPUF",
            &pin_PF4, &pin_PB5,
            thelio->timer_1, TIMER_CHANNEL_A,
            thelio->config.fan_duty, (double)thelio->config.fan_transition
        );
        device_set_callback(&thelio->cpufan, thelio_cpufan_callback, thelio);
        thelio->devices[device_i++] = &thelio->cpufan;
    }

    {
        GETPIN(F, 5); // ADC5
        GETPIN(B, 6); // OC1B
        thelio->intakefan = device_new(
            "INTF",
            &pin_PF5, &pin_PB6,
            thelio->timer_1, TIMER_CHANNEL_B,
            thelio->config.fan_duty, (double)thelio->config.fan_transition
        );
        thelio->devices[device_i++] = &thelio->intakefan;
    }

    {
        GETPIN(F, 6); // ADC6
        GETPIN(C, 6); // OC3A
        thelio->exhaustfan = device_new(
            "EXHF",
            &pin_PF6, &pin_PC6,
            thelio->timer_3, TIMER_CHANNEL_A,
            thelio->config.fan_duty, (double)thelio->config.fan_transition
        );
        thelio->devices[device_i++] = &thelio->exhaustfan;
    }

    return 0;
}

void thelio_init(struct Thelio * thelio) {
    // Initialize timers
    timer_fast_pwm(thelio->timer_1, thelio->config.fan_frequency);
    timer_fast_pwm(thelio->timer_3, thelio->config.fan_frequency);
    timer_fast_pwm(thelio->timer_4, thelio->config.led_frequency);

    // Initialize pins
    gpio_set_dir(thelio->motherled, 0);
    gpio_set(thelio->motherled, 1);
    gpio_set_dir(thelio->motherbtn, 1);
    gpio_set(thelio->motherbtn, 0);
    gpio_set_dir(thelio->cpufanmux, 1);
    gpio_set(thelio->cpufanmux, 0);

    // Get motherboard LED polarity (system is off at this point, except after firmware update)
    thelio->motherled_off = gpio_get(thelio->motherled);

    // Initialize devices
    for (int i = 0; i < sizeof(thelio->devices)/sizeof(struct Device *); i++) {
        device_init(thelio->devices[i]);
    }
}

void thelio_command(struct Thelio * thelio, uint64_t time, char * command, FILE * output) {
    uint8_t error = 1;

    if (strncmp(command, "Io", 2) == 0) {
        if (strncmp(command + 2, "TACH", 4) == 0) {
            for (int i = 0; i < sizeof(thelio->devices)/sizeof(struct Device *); i++) {
                struct Device * device = thelio->devices[i];

                if (strncmp(command + 6, device->name, 4) == 0) {
                    fprintf(output, "\r\n%04X\r\n", device->input_tach.value);

                    error = 0;

                    break;
                }
            }
        } else if (strncmp(command + 2, "DUTY", 4) == 0) {
            for (int i = 0; i < sizeof(thelio->devices)/sizeof(struct Device *); i++) {
                struct Device * device = thelio->devices[i];

                if (strncmp(command + 6, device->name, 4) == 0) {
                    if (strlen(command + 10) == 0) {
                        fprintf(output, "\r\n%04X\r\n", device->output_duty);

                        error = 0;
                    } else {
                        errno = 0;
                        unsigned long duty = strtoul(command + 10, NULL, 16);
                        if (errno == 0 && duty <= 10000) {
                            device->command_time = time;
                            if ((uint16_t)duty != device->output_trans.value) {
                                transition_set(&device->output_trans, time, (double)duty);
                            }

                            error = 0;
                        }
                    }

                    break;
                }
            }
        } else if (strncmp(command + 2, "SUSP", 4) == 0) {
            if (strlen(command + 6) == 0) {
                fprintf(output, "\r\n%04X\r\n", thelio->suspend_state);

                error = 0;
            } else {
                errno = 0;
                unsigned long suspend_state = strtoul(command + 6, NULL, 16);
                if (errno == 0 && suspend_state <= 1) {
                    thelio->suspend_state = suspend_state;

                    error = 0;
                }
            }
        } else if (strncmp(command + 2, "REVISION", 8) == 0) {
            fprintf(output, "\r\n%s\r\n", version());

            error = 0;
        } else if (strncmp(command + 2, "RSET", 4) == 0) {
            thelio->suspend_state = 0;

            // Update motherboard LED polarity (system will be on at this point)
            thelio->motherled_off = !gpio_get(thelio->motherled);

            error = 0;
        } else if (strncmp(command + 2, "BOOT", 4) == 0) {
            thelio->bootloader = 1;

            error = 0;
        }
    }

    if (error) {
        fprintf(output, "\r\nERROR\r\n");
    } else {
        fprintf(output, "\r\nOK\r\n");
    }
}

void thelio_step(struct Thelio * thelio, uint64_t time) {
    for (int i = 0; i < sizeof(thelio->devices)/sizeof(struct Device *); i++) {
        device_step(thelio->devices[i], time);
    }
}

void thelio_destroy(struct Thelio * thelio) {
    // Destroy devices
    for (int i = 0; i < sizeof(thelio->devices)/sizeof(struct Device *); i++) {
        device_destroy(thelio->devices[i]);
    }

    // Reset pins
    gpio_set_dir(thelio->motherled, 0);
    gpio_set(thelio->motherled, 0);
    gpio_set_dir(thelio->motherbtn, 0);
    gpio_set(thelio->motherbtn, 0);
    gpio_set_dir(thelio->cpufanmux, 0);
    gpio_set(thelio->cpufanmux, 0);

    // Reset timers
    timer_init(thelio->timer_1);
    timer_init(thelio->timer_3);
    timer_init(thelio->timer_4);
}

int main(int argc, char ** argv) {
    arch_init();

    static struct Thelio thelio;
    if (thelio_new(&thelio) != 0) {
        return 1;
    }

    static char usb_cmd[16];
    static int usb_cmd_i;
    memset(usb_cmd, 0, sizeof(usb_cmd)/sizeof(char));
    usb_cmd_i = 0;

    printf("running thelio\n");

    thelio_init(&thelio);

    // Initialize USB
    USBtoSerial_Init();

    static uint64_t time;
    time = time_get();
    for (;;) {
        {
            uint64_t next_time = time_get();
            uint32_t interval = (uint32_t)(next_time - time);
            if (interval > 1000) {
                printf("%ld > 1000\n", interval);
            }
            time = next_time;
        }

        thelio_step(&thelio, time);

        USBtoSerial_Task();

        if (thelio.bootloader) {
            break;
        }

        int16_t result = USBtoSerial_Read();
        if (result >= 0) {
            char c = (char)result;
            if (c == '\r') {
                usb_cmd[usb_cmd_i] = 0;

                thelio_command(&thelio, time, usb_cmd, &usb_file);

                memset(usb_cmd, 0, usb_cmd_i);
                usb_cmd_i = 0;
            } else if (usb_cmd_i + 1 < sizeof(usb_cmd)/sizeof(char)) {
                usb_cmd[usb_cmd_i++] = c;
            }
        }

        USBtoSerial_Task();

        // if (uart_can_read(stdio_uart)) {
        //     char c = uart_read(stdio_uart);
        //     if (c == 3) {
        //         break;
        //     }
        // }
    }

    USBtoSerial_Destroy();

    thelio_destroy(&thelio);

    printf("exiting thelio\n");

    USBtoSerial_Bootloader();

    return 0;
}
