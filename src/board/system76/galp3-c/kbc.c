#include <stdio.h>

#include <board/kbc.h>

void kbc_init(void) {
    *(KBC.irq) = 0;
    *(KBC.control) = 0x48;
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
                            kbc_keyboard(kbc, 0xFA);
                            break;
                        case 0xF5:
                            printf("    disable scanning\n");
                            kbc_keyboard(kbc, 0xFA);
                            break;
                        case 0xFF:
                            printf("    self test\n");
                            kbc_keyboard(kbc, 0xFA);
                            while (kbc_status(kbc) & KBC_STS_OBF) {}
                            // Yep, everything is still good, I promise
                            kbc_keyboard(kbc, 0xAA);
                            break;
                    }
                    break;
                case KBC_STATE_WRITE_CONFIG:
                    printf("  write configuration byte\n");
                    state = KBC_STATE_NORMAL;
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
