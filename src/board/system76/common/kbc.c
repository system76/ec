// SPDX-License-Identifier: GPL-3.0-only

#include <board/kbc.h>
#include <board/kbscan.h>
#include <board/keymap.h>
#include <common/debug.h>
#include <ec/ps2.h>

void kbc_init(void) {
    // Disable interrupts
    *(KBC.irq) = 0;
    *(KBC.control) = 0;
    // Set "key lock" to disabled
    *(KBC.status) = (1 << 4);
}

#define KBC_TIMEOUT 10000

// Enable first port - TODO
bool kbc_first = false;
// Enable second port - TODO
bool kbc_second = false;
// Translate from scancode set 2 to scancode set 1
// for basically no good reason
static bool kbc_translate = true;
// LED state
uint8_t kbc_leds = 0;

// Values from linux/drivers/input/keyboard/atkbd.c
static const uint16_t kbc_typematic_period[32] = {
    33,     // 30.0 cps = ~33.33ms
    37,     // 26.7 cps = ~37.45ms
    42,     // 24.0 cps = ~41.67ms
    46,     // 21.8 cps = ~45.87ms
    50,     // 20.7 cps = ~48.30ms
    54,     // 18.5 cps = ~54.05ms
    58,     // 17.1 cps = ~58.48ms
    63,     // 16.0 cps = ~62.50ms
    67,     // 15.0 cps = ~66.67ms
    75,     // 13.3 cps = ~75.19ms
    83,     // 12.0 cps = ~83.33ms
    92,     // 10.9 cps = ~91.74ms
    100,    // 10.0 cps = 100ms
    109,    //  9.2 cps = ~108.70ms
    116,    //  8.6 cps = ~116.28ms
    125,    //  8.0 cps = 125ms
    133,    //  7.5 cps = ~133.33ms
    149,    //  6.7 cps = ~149.25ms
    167,    //  6.0 cps = ~166.67ms
    182,    //  5.5 cps = ~181.82ms
    200,    //  5.0 cps = 200ms
    217,    //  4.6 cps = ~217.39ms
    232,    //  4.3 cps = ~232.56ms
    250,    //  4.0 cps = 250ms
    270,    //  3.7 cps = ~270.27ms
    303,    //  3.3 cps = ~303.03ms
    333,    //  3.0 cps = ~333.33ms
    370,    //  2.7 cps = ~370.37ms
    400,    //  2.5 cps = 400ms
    435,    //  2.3 cps = ~434.78ms
    470,    //  2.1 cps = ~478.19ms
    500,    //  2.0 cps = 500ms
};

bool kbc_scancode(struct Kbc * kbc, uint16_t key, bool pressed) {
    if (!kbc_first) return true;
    if (kbc_translate) {
        key = keymap_translate(key);
    }
    if (!key) return true;
    switch (key & 0xFF00) {
        case KF_E0:
            TRACE("  E0\n");
            if (!kbc_keyboard(kbc, 0xE0, KBC_TIMEOUT)) return false;
            key &= 0xFF;
            // Fall through
        case 0x00:
            if (!pressed) {
                if (kbc_translate) {
                    key |= 0x80;
                } else {
                    TRACE("  F0\n");
                    if (!kbc_keyboard(kbc, 0xF0, KBC_TIMEOUT)) return false;
                }
            }
            TRACE("  %02X\n", key);
            if (!kbc_keyboard(kbc, (uint8_t)key, KBC_TIMEOUT)) return false;
            break;
    }
    return true;
}

enum KbcState {
    KBC_STATE_NORMAL,
    KBC_STATE_WRITE_CONFIG,
    KBC_STATE_SET_LEDS,
    KBC_STATE_SCANCODE,
    KBC_STATE_TYPEMATIC,
    KBC_STATE_WRITE_PORT,
    KBC_STATE_FIRST_PORT_OUTPUT,
    KBC_STATE_SECOND_PORT_OUTPUT,
    KBC_STATE_SECOND_PORT_INPUT,
};

void kbc_event(struct Kbc * kbc) {
    // TODO: state per KBC (we only have one KBC so low priority)
    static enum KbcState state = KBC_STATE_NORMAL;

    uint8_t sts = kbc_status(kbc);
    if (sts & KBC_STS_IBF) {
        uint8_t data = kbc_read(kbc);
        if (sts & KBC_STS_CMD) {
            TRACE("kbc cmd: %02X\n", data);

            state = KBC_STATE_NORMAL;
            switch (data) {
            case 0x20:
                TRACE("  read configuration byte\n");
                // Interrupt enable flags
                uint8_t config = *kbc->control & 0x03;
                // System flag
                if (*kbc->status & (1 << 2)) {
                    config |= (1 << 2);
                }
                if (!kbc_first) {
                    config |= (1 << 4);
                }
                if (!kbc_second) {
                    config |= (1 << 5);
                }
                if (kbc_translate) {
                    config |= (1 << 6);
                }
                kbc_keyboard(kbc, config, KBC_TIMEOUT);
                break;
            case 0x60:
                TRACE("  write configuration byte\n");
                state = KBC_STATE_WRITE_CONFIG;
                break;
            case 0xA7:
                TRACE("  disable second port\n");
                kbc_second = false;
                break;
            case 0xA8:
                TRACE("  enable second port\n");
                kbc_second = true;
                break;
            case 0xA9:
                TRACE("  test second port\n");
                // TODO: communicate with touchpad?
                kbc_keyboard(kbc, 0x00, KBC_TIMEOUT);
                break;
            case 0xAA:
                TRACE("  test controller\n");
                // Why not pass the test?
                kbc_keyboard(kbc, 0x55, KBC_TIMEOUT);
                break;
            case 0xAB:
                TRACE("  test first port\n");
                // We _ARE_ the keyboard, so everything is good.
                kbc_keyboard(kbc, 0x00, KBC_TIMEOUT);
                break;
            case 0xAD:
                TRACE("  disable first port\n");
                kbc_first = false;
                break;
            case 0xAE:
                TRACE("  enable first port\n");
                kbc_first = true;
                break;
            case 0xD1:
                TRACE("  write port byte\n");
                state = KBC_STATE_WRITE_PORT;
                break;
            case 0xD2:
                TRACE("  write first port output\n");
                state = KBC_STATE_FIRST_PORT_OUTPUT;
                break;
            case 0xD3:
                TRACE("  write second port output\n");
                state = KBC_STATE_SECOND_PORT_OUTPUT;
                break;
            case 0xD4:
                TRACE("  write second port input\n");
                state = KBC_STATE_SECOND_PORT_INPUT;
                break;
            }
        } else {
            TRACE("kbc data: %02X\n", data);

            switch (state) {
                case KBC_STATE_NORMAL:
                    TRACE("  keyboard command\n");
                    switch (data) {
                        case 0xED:
                            TRACE("    set leds\n");
                            state = KBC_STATE_SET_LEDS;
                            kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT);
                            break;
                        case 0xEE:
                            TRACE("    echo\n");
                            // Hey, this is easy. I like easy commands
                            kbc_keyboard(kbc, 0xEE, KBC_TIMEOUT);
                            break;
                        case 0xF0:
                            TRACE("    get/set scancode\n");
                            state = KBC_STATE_SCANCODE;
                            kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT);
                            break;
                        case 0xF2:
                            TRACE("    identify keyboard\n");
                            if (kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT)) {
                                if (kbc_keyboard(kbc, 0xAB, KBC_TIMEOUT)) {
                                    kbc_keyboard(kbc, 0x83, KBC_TIMEOUT);
                                }
                            }
                            break;
                        case 0xF3:
                            TRACE("    set typematic rate/delay\n");
                            state = KBC_STATE_TYPEMATIC;
                            kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT);
                            break;
                        case 0xF4:
                            TRACE("    enable scanning\n");
                            kbscan_enabled = true;
                            kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT);
                            break;
                        case 0xF5:
                            TRACE("    disable scanning\n");
                            kbscan_enabled = false;
                            kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT);
                            break;
                        case 0xF6:
                            TRACE("    set default parameters\n");
                            kbc_leds = 0;
                            kbscan_repeat_period = 91;
                            kbscan_repeat_delay = 500;
                            kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT);
                            break;
                        case 0xFF:
                            TRACE("    self test\n");
                            if (kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT)) {
                                // Yep, everything is still good, I promise
                                kbc_keyboard(kbc, 0xAA, KBC_TIMEOUT);
                            }
                            break;
                    }
                    break;
                case KBC_STATE_WRITE_CONFIG:
                    TRACE("  write configuration byte\n");
                    state = KBC_STATE_NORMAL;
                    // Enable keyboard interrupt
                    if (data & 1) {
                        *kbc->control |= 1;
                    } else {
                        *kbc->control &= ~1;
                    }
                    // Enable mouse interrupt
                    if (data & (1 << 1)) {
                        *kbc->control |= (1 << 1);
                    } else {
                        *kbc->control &= ~(1 << 1);
                    }
                    // System flag
                    if (data & (1 << 2)) {
                        *kbc->status |= (1 << 2);
                    } else {
                        *kbc->status &= ~(1 << 2);
                    }
                    kbc_first = (bool)(!(data & (1 << 4)));
                    kbc_second = (bool)(!(data & (1 << 5)));
                    kbc_translate = (bool)(data & (1 << 6));
                    break;
                case KBC_STATE_SET_LEDS:
                    TRACE("  set leds\n");
                    state = KBC_STATE_NORMAL;
                    kbc_leds = data;
                    kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT);
                    break;
                case KBC_STATE_SCANCODE:
                    TRACE("  get/set scancode\n");
                    state = KBC_STATE_NORMAL;
                    #if LEVEL >= LEVEL_TRACE
                        switch (data) {
                            case 0x02:
                                TRACE("    set scancode set 2\n");
                                break;
                        }
                    #endif
                    kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT);
                    break;
                case KBC_STATE_TYPEMATIC:
                    TRACE("  set typematic rate/delay\n");
                    state = KBC_STATE_NORMAL;
                    {
                        // Rate: bits 0-4
                        uint16_t period = kbc_typematic_period[data & 0x1F];
                        kbscan_repeat_period = period;

                        // Delay: bits 5-6
                        static const uint16_t delay[4] = {250, 500, 750, 1000};
                        uint8_t idx = (data & 0x60) >> 5;
                        kbscan_repeat_delay = delay[idx];
                    }
                    kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT);
                    break;
                case KBC_STATE_WRITE_PORT:
                    TRACE("  write port byte\n");
                    state = KBC_STATE_NORMAL;
                    break;
                case KBC_STATE_FIRST_PORT_OUTPUT:
                    TRACE("  write first port output\n");
                    state = KBC_STATE_NORMAL;
                    kbc_keyboard(kbc, data, KBC_TIMEOUT);
                    break;
                case KBC_STATE_SECOND_PORT_OUTPUT:
                    TRACE("  write second port output\n");
                    state = KBC_STATE_NORMAL;
                    kbc_mouse(kbc, data, KBC_TIMEOUT);
                    break;
                case KBC_STATE_SECOND_PORT_INPUT:
                    TRACE("  write second port input\n");
                    state = KBC_STATE_NORMAL;
                    ps2_write(&PS2_3, &data, 1);
                    break;
            }
        }
    }
}
