#include <stdio.h>

#include <board/kbc.h>
#include <board/kbscan.h>
#include <board/keymap.h>

void kbc_init(void) {
    // Disable interrupts
    *(KBC.irq) = 0;
    *(KBC.control) = 0;
}

// System flag
static bool kbc_system = false;
// Translate from scancode set 2 to scancode set 1
// for basically no good reason
static bool kbc_translate = true;

void kbc_key(struct Kbc * kbc, uint16_t key, bool pressed) {
    if (kbc_translate) {
        key = keymap_translate(key);
    }
    switch (key & 0xFF00) {
        case K_E0:
            printf("  E0\n");
            kbc_keyboard(kbc, 0xE0);
            key &= 0xFF;
            // Fall through
        case 0x00:
            if (!pressed) {
                if (kbc_translate) {
                    key |= 0x80;
                } else {
                    printf("  F0\n");
                    kbc_keyboard(kbc, 0xF0);
                }
            }
            printf("  %02X\n", key);
            kbc_keyboard(kbc, (uint8_t)key);
            break;
    }
}

enum KbcState {
    KBC_STATE_NORMAL,
    KBC_STATE_WRITE_CONFIG,
    KBC_STATE_SET_LEDS,
    KBC_STATE_SCANCODE,
};

void kbc_event(struct Kbc * kbc) {
    // TODO: state per KBC (we only have one KBC so low priority)
    static enum KbcState state = KBC_STATE_NORMAL;

    uint8_t sts = kbc_status(kbc);
    if (sts & KBC_STS_IBF) {
        uint8_t data = kbc_read(kbc);
        if (sts & KBC_STS_CMD) {
            printf("kbc cmd: %02X\n", data);

            state = KBC_STATE_NORMAL;
            switch (data) {
            case 0x20:
                printf("  read configuration byte\n");
                uint8_t config = *kbc->control & 0x03;
                if (kbc_system) {
                    config |= (1 << 2);
                }
                if (kbc_translate) {
                    config |= (1 << 6);
                }
                break;
            case 0x60:
                printf("  write configuration byte\n");
                state = KBC_STATE_WRITE_CONFIG;
                break;
            case 0xA7:
                printf("  disable second port\n");
                break;
            case 0xA8:
                printf("  enable second port\n");
                break;
            case 0xAA:
                printf("  test controller\n");
                // Why not pass the test?
                kbc_keyboard(kbc, 0x55);
                break;
            case 0xAB:
                printf("  test first port\n");
                // We _ARE_ the keyboard, so everything is good.
                kbc_keyboard(kbc, 0x00);
                break;
            case 0xAD:
                printf("  disable first port\n");
                break;
            case 0xAE:
                printf("  enable first port\n");
                break;
            }
        } else {
            printf("kbc data: %02X\n", data);

            switch (state) {
                case KBC_STATE_NORMAL:
                    printf("  keyboard command\n");
                    switch (data) {
                        case 0xED:
                            printf("    set leds\n");
                            state = KBC_STATE_SET_LEDS;
                            kbc_keyboard(kbc, 0xFA);
                            break;
                        case 0xEE:
                            printf("    echo\n");
                            // Hey, this is easy. I like easy commands
                            kbc_keyboard(kbc, 0xEE);
                            break;
                        case 0xF0:
                            printf("    get/set scancode\n");
                            state = KBC_STATE_SCANCODE;
                            kbc_keyboard(kbc, 0xFA);
                            break;
                        case 0xF4:
                            printf("    enable scanning\n");
                            kbscan_enabled = true;
                            kbc_keyboard(kbc, 0xFA);
                            break;
                        case 0xF5:
                            printf("    disable scanning\n");
                            kbscan_enabled = false;
                            kbc_keyboard(kbc, 0xFA);
                            break;
                        case 0xFF:
                            printf("    self test\n");
                            kbc_keyboard(kbc, 0xFA);
                            // Yep, everything is still good, I promise
                            kbc_keyboard(kbc, 0xAA);
                            break;
                    }
                    break;
                case KBC_STATE_WRITE_CONFIG:
                    printf("  write configuration byte\n");
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
                    if (data & (1 << 2)) {
                        kbc_system = true;
                    } else {
                        kbc_system = false;
                    }
                    if (data & (1 << 6)) {
                        kbc_translate = true;
                    } else {
                        kbc_translate = false;
                    }
                    *kbc->control = control;
                    break;
                case KBC_STATE_SET_LEDS:
                    printf("  set leds\n");
                    state = KBC_STATE_NORMAL;
                    kbc_keyboard(kbc, 0xFA);
                    break;
                case KBC_STATE_SCANCODE:
                    printf("  get/set scancode\n");
                    state = KBC_STATE_NORMAL;
                    switch (data) {
                        case 0x02:
                            printf("    set scancode set 2\n");
                            break;
                    }
                    kbc_keyboard(kbc, 0xFA);
                    break;
            }
        }
    }
}
