#include <stdio.h>

#include <arch/delay.h>
#include <board/kbc.h>
#include <board/kbscan.h>
#include <board/keymap.h>

bool kbscan_enabled = false;
uint8_t kbscan_layer = 0;
uint8_t __pdata kbscan_last[KM_OUT] = { 0 };

void kbscan_init(void) {
    KSOCTRL = 0x05;
    KSICTRLR = 0x04;

    // Set all outputs to GPIO mode and low
    KSOL = 0;
    KSOH1 = 0;
    KSOH2 = 0;
    KSOLGCTRL = 0xFF;
    KSOLGOEN = 0xFF;
    KSOHGCTRL = 0xFF;
    KSOHGOEN = 0xFF;
}

void kbscan_event(void) {
    int i;
    for (i = 0; i < KM_OUT; i++) {
        KSOL = 0xFF;
        KSOH1 = 0xFF;
        KSOH2 = 0xFF;
        if (i < 8) {
            KSOL = ~(1 << i);
        } else if (i < 16) {
            KSOH1 = ~(1 << (i - 8));
        } else if (i < 24) {
            KSOH2 = ~(1 << (i - 16));
        }

        // TODO: figure out optimal delay
        delay_ticks(1);

        uint8_t new = ~KSI;
        uint8_t last = kbscan_last[i];
        if (new != last) {
            int j;
            for (j = 0; j < 8; j++) {
                bool new_b = new & (1 << j);
                bool last_b = last & (1 << j);
                if (new_b != last_b) {
                    uint16_t key = keymap(i, j, kbscan_layer);
                    printf("KB %d, %d, %d = 0x%04X, %d\n", i, j, kbscan_layer, key, new_b);
                    if (kbscan_enabled && key) {
                        switch (key & 0xFF00) {
                            case K_E0:
                                printf("  E0\n");
                                kbc_keyboard(&KBC, 0xE0);
                                // Fall through
                            case 0x00:
                                if (!new_b) {
                                    printf("  F0\n");
                                    kbc_keyboard(&KBC, 0xF0);
                                }
                                uint8_t key_byte = (uint8_t)(key & 0xFF);
                                printf("  %02X\n", key_byte);
                                kbc_keyboard(&KBC, key_byte);
                                break;
                        }
                    }
                }
            }

            kbscan_last[i] = new;
        }
    }

    KSOL = 0;
    KSOH1 = 0;
    KSOH2 = 0;

    // TODO: figure out optimal delay
    delay_ticks(1);
}
