#include <board/kbc.h>
#include <board/kbscan.h>
#include <board/keymap.h>
#include <common/debug.h>
#include <ec/ps2.h>

void kbc_init(void) {
    // Disable interrupts
    *(KBC.irq) = 0;
    *(KBC.control) = 0;
}

#define KBC_TIMEOUT 1000

// System flag
static bool kbc_system = false;
// Enable first port - TODO
bool kbc_first = false;
// Enable second port - TODO
bool kbc_second = false;
// Translate from scancode set 2 to scancode set 1
// for basically no good reason
static bool kbc_translate = true;

bool kbc_scancode(struct Kbc * kbc, uint16_t key, bool pressed) {
    if (!kbc_first) return true;
    if (kbc_translate) {
        key = keymap_translate(key);
    }
    if (!key) return true;
    switch (key & 0xFF00) {
        case K_E0:
            DEBUG("  E0\n");
            if (!kbc_keyboard(kbc, 0xE0, KBC_TIMEOUT)) return false;
            key &= 0xFF;
            // Fall through
        case 0x00:
            if (!pressed) {
                if (kbc_translate) {
                    key |= 0x80;
                } else {
                    DEBUG("  F0\n");
                    if (!kbc_keyboard(kbc, 0xF0, KBC_TIMEOUT)) return false;
                }
            }
            DEBUG("  %02X\n", key);
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
            DEBUG("kbc cmd: %02X\n", data);

            state = KBC_STATE_NORMAL;
            switch (data) {
            case 0x20:
                DEBUG("  read configuration byte\n");
                uint8_t config = *kbc->control & 0x03;
                if (kbc_system) {
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
                DEBUG("  write configuration byte\n");
                state = KBC_STATE_WRITE_CONFIG;
                break;
            case 0xA7:
                DEBUG("  disable second port\n");
                kbc_second = false;
                break;
            case 0xA8:
                DEBUG("  enable second port\n");
                kbc_second = true;
                break;
            case 0xA9:
                DEBUG("  test second port\n");
                // TODO: communicate with touchpad?
                kbc_keyboard(kbc, 0x00, KBC_TIMEOUT);
                break;
            case 0xAA:
                DEBUG("  test controller\n");
                // Why not pass the test?
                kbc_keyboard(kbc, 0x55, KBC_TIMEOUT);
                break;
            case 0xAB:
                DEBUG("  test first port\n");
                // We _ARE_ the keyboard, so everything is good.
                kbc_keyboard(kbc, 0x00, KBC_TIMEOUT);
                break;
            case 0xAD:
                DEBUG("  disable first port\n");
                kbc_first = false;
                break;
            case 0xAE:
                DEBUG("  enable first port\n");
                kbc_first = true;
                break;
            case 0xD1:
                DEBUG("  write port byte\n");
                state = KBC_STATE_WRITE_PORT;
                break;
            case 0xD2:
                DEBUG("  write first port output\n");
                state = KBC_STATE_FIRST_PORT_OUTPUT;
                break;
            case 0xD3:
                DEBUG("  write second port output\n");
                state = KBC_STATE_SECOND_PORT_OUTPUT;
                break;
            case 0xD4:
                DEBUG("  write second port input\n");
                state = KBC_STATE_SECOND_PORT_INPUT;
                break;
            }
        } else {
            DEBUG("kbc data: %02X\n", data);

            switch (state) {
                case KBC_STATE_NORMAL:
                    DEBUG("  keyboard command\n");
                    switch (data) {
                        case 0xED:
                            DEBUG("    set leds\n");
                            state = KBC_STATE_SET_LEDS;
                            kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT);
                            break;
                        case 0xEE:
                            DEBUG("    echo\n");
                            // Hey, this is easy. I like easy commands
                            kbc_keyboard(kbc, 0xEE, KBC_TIMEOUT);
                            break;
                        case 0xF0:
                            DEBUG("    get/set scancode\n");
                            state = KBC_STATE_SCANCODE;
                            kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT);
                            break;
                        case 0xF2:
                            DEBUG("    identify keyboard\n");
                            if (kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT)) {
                                if (kbc_keyboard(kbc, 0xAB, KBC_TIMEOUT)) {
                                    kbc_keyboard(kbc, 0x83, KBC_TIMEOUT);
                                }
                            }
                            break;
                        case 0xF4:
                            DEBUG("    enable scanning\n");
                            kbscan_enabled = true;
                            kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT);
                            break;
                        case 0xF5:
                            DEBUG("    disable scanning\n");
                            kbscan_enabled = false;
                            kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT);
                            break;
                        case 0xFF:
                            DEBUG("    self test\n");
                            if (kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT)) {
                                // Yep, everything is still good, I promise
                                kbc_keyboard(kbc, 0xAA, KBC_TIMEOUT);
                            }
                            break;
                    }
                    break;
                case KBC_STATE_WRITE_CONFIG:
                    DEBUG("  write configuration byte\n");
                    state = KBC_STATE_NORMAL;
                    uint8_t control = *kbc->control;
                    if (data & 1) {
                        control |= 1;
                    } else {
                        control &= ~1;
                    }
                    if (data & (1 << 1)) {
                        control |= (1 << 1);
                    } else {
                        control &= ~(1 << 1);
                    }
                    kbc_system = (bool)(data & (1 << 2));
                    kbc_first = (bool)(!(data & (1 << 4)));
                    kbc_second = (bool)(!(data & (1 << 5)));
                    kbc_translate = (bool)(data & (1 << 6));
                    *kbc->control = control;
                    break;
                case KBC_STATE_SET_LEDS:
                    DEBUG("  set leds\n");
                    state = KBC_STATE_NORMAL;
                    kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT);
                    break;
                case KBC_STATE_SCANCODE:
                    DEBUG("  get/set scancode\n");
                    state = KBC_STATE_NORMAL;
                    #if LEVEL >= LEVEL_DEBUG
                        switch (data) {
                            case 0x02:
                                DEBUG("    set scancode set 2\n");
                                break;
                        }
                    #endif
                    kbc_keyboard(kbc, 0xFA, KBC_TIMEOUT);
                    break;
                case KBC_STATE_WRITE_PORT:
                    DEBUG("  write port byte\n");
                    state = KBC_STATE_NORMAL;
                    break;
                case KBC_STATE_FIRST_PORT_OUTPUT:
                    DEBUG("  write first port output\n");
                    state = KBC_STATE_NORMAL;
                    kbc_keyboard(kbc, data, KBC_TIMEOUT);
                    break;
                case KBC_STATE_SECOND_PORT_OUTPUT:
                    DEBUG("  write second port output\n");
                    state = KBC_STATE_NORMAL;
                    kbc_mouse(kbc, data, KBC_TIMEOUT);
                    break;
                case KBC_STATE_SECOND_PORT_INPUT:
                    DEBUG("  write second port input\n");
                    state = KBC_STATE_NORMAL;
                    ps2_write(&PS2_3, &data, 1);
                    break;
            }
        }
    }
}
