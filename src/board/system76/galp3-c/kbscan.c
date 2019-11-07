#include <stdio.h>

#include <arch/delay.h>
#include <board/kbscan.h>

void kbscan_init(void) {
    KSOCTRL = 0x05;
    KSICTRLR = 0x04;

    // Set all outputs to GPIO mode and low
    KSOH2 = 0;
    KSOH1 = 0;
    KSOL = 0;
    KSOHGCTRL = 0xFF;
    KSOHGOEN = 0xFF;
    KSOLGCTRL = 0xFF;
    KSOLGOEN = 0xFF;
}

void kbscan_event(void) {
    static uint8_t last = 0xFF;
    uint8_t new = KSI;
    if (new != last) {
        printf("KSI %02X\n", new);

        int i;
        for (i = 0; i <= 15; i++) {
            if (i < 8) {
                KSOL = ~(1 << i);
                KSOH1 = 0xFF;
            } else {
                KSOL = 0xFF;
                KSOH1 = ~(1 << (i - 8));
            }

            // TODO: figure out optimal delay
            delay_ms(1);

            uint8_t ksi = KSI;

            printf("  %d: %02X\n", i, ksi);
            int j;
            for (j = 0; j < 8; j++) {
                if (!(ksi & (1 << j))) {
                    printf("    %d, %d\n", i, j);
                }
            }
        }

        KSOL = 0;
        KSOH1 = 0;

        delay_ms(1);
    }
    last = new;
}
