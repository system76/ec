// SPDX-License-Identifier: GPL-3.0-only

#include <arch/delay.h>
#include <arch/time.h>
#include <common/debug.h>
#include <ec/adc.h>

// Based on chromium EC chip/it83xx/adc.c

void adc_init(void) {
    ADCSTS &= ~BIT(7);
    // Start analog accuracy initialization
    ADCSTS |= BIT(3);
    // Enable automatic hardware calibration
    KDCTL |= BIT(7);
    // Short delay for ADC accuracy initialization
    delay_ms(1);
    // Stop analog accuracy initialization
    ADCSTS &= ~BIT(3);
}

// Enable measurement of an input on a specified channel
void adc_enable_channel(uint8_t ch) {
    if (ch < 4) {
        *adc_channels[ch].control = (0xa0 | ch);
    } else {
        *adc_channels[ch].control = 0xb0;
    }
    ADCCFG |= BIT(0);
}

void adc_disable_channel(uint8_t ch) {
    if (ch < 4) {
        *adc_channels[ch].control = 0x9f;
    } else {
        *adc_channels[ch].control = 0x80;
    }
    ADCCFG &= BIT(0);
}

bool adc_data_valid(uint8_t ch) {
    return *adc_channels[ch].control & BIT(7);
}

// TODO what's a good timeout here? Chromium has 1ms
#define ADC_VALID_TIMEOUT 1000
// Max measurement of 3V
#define ADC_MAX 3000

// Measure voltage value on a specified ADC channel. Returns voltage in mV from
// 0V to Vcc/1.1 and <0 on error
int16_t adc_read_channel(uint8_t ch) {
    adc_enable_channel(ch);

    // Wait for channel to become valid
    uint32_t start = time_get();
    while (!adc_data_valid(ch)) {
        if ((time_get() - start) >= ADC_VALID_TIMEOUT) {
            DEBUG("adc_read_channel: timed out waiting for measurement!\n");
            return -1;
        }
    };

    uint32_t measurement = (((*adc_channels[ch].data_hi) & 0x03) << 8)
                            | *adc_channels[ch].data_lo;

    adc_disable_channel(ch);

    return (measurement * ADC_MAX) / 0x3FF;
}
