#include <mcs51/8052.h>

#include <arch/delay.h>
#include <board/gpio.h>
#include <board/kbc.h>
#include <board/kbscan.h>
#include <board/keymap.h>
#include <board/pmc.h>
#include <common/debug.h>

bool kbscan_enabled = false;

void kbscan_init(void) {
    KSOCTRL = 0x05;
    KSICTRLR = 0x04;

    // Set all outputs to GPIO mode, low, and inputs
    KSOL = 0;
    KSOLGCTRL = 0xFF;
    KSOLGOEN = 0;
    KSOH1 = 0;
    KSOHGCTRL = 0xFF;
    KSOHGOEN = 0;
    KSOH2 = 0;

    // Make sure timer 2 is stopped
    T2CON = 0;
}

void kbscan_event(void) {
    static uint8_t kbscan_layer = 0;
    uint8_t layer = kbscan_layer;
    static uint8_t kbscan_last[KM_OUT] = { 0 };

    // If timer 2 is finished
    if (TF2) {
        // Stop timer 2 running
        TR2 = 0;
        // Clear timer 2 finished flag
        TF2 = 0;
    }

    int i;
    for (i = 0; i < KM_OUT; i++) {
        // Set current line as output
        if (i < 8) {
            KSOLGOEN = 0;
            KSOLGOEN = 1 << i;
            GPCRC3 = GPIO_IN;
            GPCRC5 = GPIO_IN;
        } else if (i < 16) {
            KSOLGOEN = 0;
            KSOHGOEN = 1 << (i - 8);
            GPCRC3 = GPIO_IN;
            GPCRC5 = GPIO_IN;
        } else if (i == 16) {
            KSOLGOEN = 0;
            KSOHGOEN = 0;
            GPCRC3 = GPIO_OUT;
            GPCRC5 = GPIO_IN;
        } else if (i == 17) {
            KSOLGOEN = 0;
            KSOHGOEN = 0;
            GPCRC3 = GPIO_IN;
            GPCRC5 = GPIO_OUT;
        }
        GPDRC &= ~((1 << 3) | (1 << 5));

        // TODO: figure out optimal delay
        delay_ticks(10);

        uint8_t new = ~KSI;
        uint8_t last = kbscan_last[i];
        if (new != last) {
            int j;
            for (j = 0; j < KM_IN; j++) {
                bool new_b = new & (1 << j);
                bool last_b = last & (1 << j);
                if (new_b != last_b) {
                    // If timer 2 is running
                    if (TR2) {
                        // Debounce releases
                        if (!new_b) {
                            // Restore bit, so that this release can be handled later
                            new |= (1 << j);
                            // Skip processing of release
                            continue;
                        }
                    } else {
                        // Begin debouncing on press
                        if (new_b) {
                            // Run timer 2 for 20 ms
                            // 65536-(20000 * 69 + 89)/90 = 0xC419
                            TH2 = 0xC4;
                            TL2 = 0x19;
                            TR2 = 1;
                        }
                    }

                    uint16_t key = keymap(i, j, kbscan_layer);
                    DEBUG("KB %d, %d, %d = 0x%04X, %d\n", i, j, kbscan_layer, key, new_b);
                    if (!key) {
                        WARN("KB %d, %d, %d missing\n", i, j, kbscan_layer);
                    }
                    switch (key & KT_MASK) {
                        case (KT_FN):
                            if (new_b) layer = 1;
                            else layer = 0;
                            break;
                        case (KT_SCI):
                            if (new_b) {
                                uint8_t sci = (uint8_t)(key & 0xFF);
                                if (!pmc_sci(&PMC_1, sci)) {
                                    // In the case of ignored SCI, reset bit
                                    new &= ~(1 << j);
                                }
                            }
                            break;
                        case (KT_NORMAL):
                            if (kbscan_enabled && key) {
                                kbc_scancode(&KBC, key, new_b);
                            }
                            break;
                    }
                }
            }

            kbscan_last[i] = new;
        }
    }

    if (layer != kbscan_layer) {
        //TODO: unpress keys before going to scratch rom

        // Unpress all currently pressed keys
        for (i = 0; i < KM_OUT; i++) {
            uint8_t new = 0;
            uint8_t last = kbscan_last[i];
            if (last) {
                int j;
                for (j = 0; j < KM_IN; j++) {
                    bool new_b = new & (1 << j);
                    bool last_b = last & (1 << j);
                    if (new_b != last_b) {
                        uint16_t key = keymap(i, j, kbscan_layer);
                        DEBUG("KB %d, %d, %d = 0x%04X, %d\n", i, j, kbscan_layer, key, new_b);
                        switch (key & KT_MASK) {
                            case (KT_NORMAL):
                                if (kbscan_enabled && key) {
                                    kbc_scancode(&KBC, key, new_b);
                                }
                                break;
                        }
                    }
                }
            }

            kbscan_last[i] = new;
        }

        kbscan_layer = layer;
    }

    // Reset all lines to inputs
    KSOLGOEN = 0;
    KSOHGOEN = 0;
    GPCRC3 = GPIO_IN;
    GPCRC5 = GPIO_IN;

    // TODO: figure out optimal delay
    delay_ticks(10);
}
