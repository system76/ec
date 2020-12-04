// SPDX-License-Identifier: GPL-3.0-only

#include <arch/delay.h>
#include <arch/time.h>
#include <board/acpi.h>
#include <board/battery.h>
#include <board/board.h>
#include <board/config.h>
#include <board/fan.h>
#include <board/gpio.h>
#include <board/kbled.h>
#include <board/lid.h>
#include <board/power.h>
#include <board/pmc.h>
#include <board/pnp.h>
#include <common/debug.h>

#include <ec/espi.h>
#if EC_ESPI
    #include <board/espi.h>
#endif

#define GPIO_SET_DEBUG(G, V) { \
    DEBUG("%s = %s\n", #G, V ? "true" : "false"); \
    gpio_set(&G, V); \
}

#ifndef DEEP_SX
    // Platform does not currently support Deep Sx
    #define DEEP_SX 0
#endif

#ifndef HAVE_EC_EN
    #define HAVE_EC_EN 1
#endif

#ifndef HAVE_LAN_WAKEUP_N
    #define HAVE_LAN_WAKEUP_N 1
#endif

#ifndef HAVE_LED_BAT_CHG
    #define HAVE_LED_BAT_CHG 1
#endif

#ifndef HAVE_LED_BAT_FULL
    #define HAVE_LED_BAT_FULL 1
#endif

#ifndef HAVE_PCH_DPWROK_EC
    #define HAVE_PCH_DPWROK_EC 1
#endif

#ifndef HAVE_PCH_PWROK_EC
    #define HAVE_PCH_PWROK_EC 1
#endif

#ifndef HAVE_SLP_SUS_N
    #define HAVE_SLP_SUS_N 1
#endif

#ifndef HAVE_XLP_OUT
    #define HAVE_XLP_OUT 1
#endif
#ifndef HAVE_SUSWARN_N
    #define HAVE_SUSWARN_N 1
#endif

#ifndef HAVE_SUS_PWR_ACK
    #define HAVE_SUS_PWR_ACK 1
#endif

#ifndef HAVE_VA_EC_EN
    #define HAVE_VA_EC_EN 1
#endif

#ifndef HAVE_XLP_OUT
    #define HAVE_XLP_OUT 1
#endif

extern uint8_t main_cycle;

// VccRTC stable (55%) to RTCRST# high
#define tPCH01 delay_ms(9)
// VccDSW stable (95%) to RSMRST# high
#define tPCH02 delay_ms(10)
// VccPrimary stable (95%) to RSMRST# high
#define tPCH03 delay_ms(10)
// VccRTC stable (90%) to start of VccDSW voltage ramp
#define tPCH04 delay_ms(9)
// RTCRST# high to DSW_PWROK
#define tPCH05 delay_us(1)
// VccDSW 3.3 stable to VccPrimary 1.05V
#define tPCH06 delay_us(200)
// DSW_PWROK high to RSMRST# high
#define tPCH07 delay_ms(0)
// SLP_S3# de-assertion to PCH_PWROK assertion
#define tPCH08 delay_ms(1)
// SLP_A# high when ASW rails are stable (95%)
#define tPCH09 delay_ms(2, 4, 8, 16) //TODO
// PCH_PWROK low to VCCIO dropping 5%
#define tPCH10 delay_ns(400)
// SLP_SUS# asserting to VccPRIM dropping 5%
#define tPCH11 delay_ns(100)
// RSMRST# asserting to VccPRIM dropping 5%
#define tPCH12 delay_ns(400)
// DSW_PWROK falling to any of VccDSW, VccPRIM dropping 5%
#define tPCH14 delay_ns(400)
// De-assertion of RSMRST# to de-assertion of ESPI_RESET#
#if DEEP_SX
    #define tPCH18 delay_us(90)
#else
    #define tPCH18 delay_ms(95)
#endif
// DSW_PWROK assertion to SLP_SUS# de-assertion
#define tPCH32 delay_ms(95)
// RSMRST# de-assertion to SUSPWRDNACK valid
#define tPLT01 delay_ms(200)

enum PowerState power_state = POWER_STATE_DEFAULT;

enum PowerState calculate_power_state(void) {
    //TODO: Deep Sx states using SLP_SUS#

    if (gpio_get(&SUSB_N_PCH)) {
        // S3, S4, and S5 planes powered
        return POWER_STATE_S0;
    }

    if (gpio_get(&SUSC_N_PCH)) {
        // S4 and S5 planes powered
        return POWER_STATE_S3;
    }

    if (gpio_get(&EC_RSMRST_N)) {
        // S5 plane powered
        return POWER_STATE_S5;
    }

#if HAVE_PCH_DPWROK_EC && DEEP_SX
    if (!gpio_get(&PCH_DPWROK_EC)) {
        return POWER_STATE_DEFAULT;
    }
#endif // HAVE_PCH_DPWROK_EC && DEEP_SX

    return POWER_STATE_DS5;
}

void update_power_state(void) {
    enum PowerState new_power_state = calculate_power_state();
    if (power_state != new_power_state) {
        power_state = new_power_state;

    #if LEVEL >= LEVEL_DEBUG
        switch (power_state) {
            case POWER_STATE_DEFAULT:
                DEBUG("POWER_STATE_DEFAULT\n");
                break;
            case POWER_STATE_DS5:
                DEBUG("POWER_STATE_DS5\n");
                break;
            case POWER_STATE_S5:
                DEBUG("POWER_STATE_S5\n");
                break;
            case POWER_STATE_DS3:
                DEBUG("POWER_STATE_DS3\n");
                break;
            case POWER_STATE_S3:
                DEBUG("POWER_STATE_S3\n");
                break;
            case POWER_STATE_S0:
                DEBUG("POWER_STATE_S0\n");
                break;
        }
    #endif
    }
}

// Enable deep sleep well power
void power_on_ds5(void) {
    DEBUG("%02X: power_on_ds5\n", main_cycle);

#if DEEP_SX
    // See Figure 12-18 in Whiskey Lake Platform Design Guide
    // | VCCRTC | RTCRST# | VCCDSW_3P3 | DSW_PWROK |
    // | tPCH01---------- |            |           |
    // | tPCH04----------------------- |           |
    // |        | tPCH05-------------------------- |
    // |        |         | tPCH02---------------- |

    // tPCH01 and tPCH02 combined make the longest delay
    tPCH01;
    tPCH02;

#if HAVE_PCH_DPWROK_EC
    // Deep sleep well is a-ok
    GPIO_SET_DEBUG(PCH_DPWROK_EC, true);
#endif // HAVE_PCH_DPWROK_EC
    // Wait for deep sleep well to propogate
    tPCH32;
#else // DEEP_SX
    // See Figure 12-19 in Whiskey Lake Platform Design Guide
    // | VCCRTC | RTCRST# | VccPRIM |
    // | tPCH01---------- |         |
    // | tPCH04-------------------- |

    // tPCH04 is the ideal delay
    tPCH04;
#endif // DEEP_SX

    update_power_state();
}

// Enable S5 power
void power_on_s5(void) {
    DEBUG("%02X: power_on_s5\n", main_cycle);

#if DEEP_SX
    // See Figure 12-18 in Whiskey Lake Platform Design Guide
    // TODO - signal timing graph
    // See Figure 12-24 in Whiskey Lake Platform Design Guide
    // TODO - rail timing graph

    // TODO: Must have SL_SUS# set high by PCH

#if HAVE_VA_EC_EN
    // Enable VCCPRIM_* planes - must be enabled prior to USB power in order to
    // avoid leakage
    GPIO_SET_DEBUG(VA_EC_EN, true);
#endif // HAVE_VA_EC_EN
    tPCH06;

    // Enable VDD5
    GPIO_SET_DEBUG(DD_ON, true);

    //TODO: Should SUS_ACK# be de-asserted here?
    tPCH03;

    // De-assert RSMRST#
    GPIO_SET_DEBUG(EC_RSMRST_N, true);

    // Wait for PCH stability
    tPCH18;

#if HAVE_EC_EN
    // Allow processor to control SUSB# and SUSC#
    GPIO_SET_DEBUG(EC_EN, true);
#endif // HAVE_EC_EN

    // Extra wait - TODO remove
    delay_ms(200);
#else // DEEP_SX
    // See Figure 12-19 in Whiskey Lake Platform Design Guide
    // TODO - signal timing graph
    // See Figure 12-25 in Whiskey Lake Platform Design Guide
    // TODO - rail timing graph

#if HAVE_VA_EC_EN
    // Enable VCCPRIM_* planes - must be enabled prior to USB power in order to
    // avoid leakage
    GPIO_SET_DEBUG(VA_EC_EN, true);
#endif // HAVE_VA_EC_EN
    tPCH06;

    // Enable VDD5
    GPIO_SET_DEBUG(DD_ON, true);

#if HAVE_SUS_PWR_ACK
    // De-assert SUS_ACK# - TODO is this needed on non-dsx?
    GPIO_SET_DEBUG(SUS_PWR_ACK, true);
#endif // HAVE_SUS_PWR_ACK
    tPCH03;

#if HAVE_PCH_DPWROK_EC
    // Assert DSW_PWROK
    GPIO_SET_DEBUG(PCH_DPWROK_EC, true);
#endif // HAVE_PCH_DPWROK_EC

    // De-assert RSMRST#
    GPIO_SET_DEBUG(EC_RSMRST_N, true);

    // Wait for PCH stability
    tPCH18;

#if HAVE_EC_EN
    // Allow processor to control SUSB# and SUSC#
    GPIO_SET_DEBUG(EC_EN, true);
#endif // HAVE_EC_EN

    // Wait for SUSPWRDNACK validity
    tPLT01;

    for (int i = 0; i < 1000; i++) {
        // If we reached S0, exit this loop
        update_power_state();
        if (power_state == POWER_STATE_S0) {
            break;
        }

        // Check for VW changes
        #if EC_ESPI
            espi_event();
        #endif // EC_ESPI

        // Extra wait until SUSPWRDNACK is valid
        delay_ms(1);
    }
#endif // DEEP_SX

    update_power_state();
}

void power_off_s5(void) {
    DEBUG("%02X: power_off_s5\n", main_cycle);

#if DEEP_SX
    // TODO
#else // DEEP_SX
#if HAVE_PCH_PWROK_EC
    // De-assert SYS_PWROK
    GPIO_SET_DEBUG(PCH_PWROK_EC, false);
#endif // HAVE_PCH_PWROK_EC

    // De-assert PCH_PWROK
    GPIO_SET_DEBUG(PM_PWROK, false);

#if HAVE_EC_EN
    // Block processor from controlling SUSB# and SUSC#
    GPIO_SET_DEBUG(EC_EN, false);
#endif // HAVE_EC_EN

    // De-assert RSMRST#
    GPIO_SET_DEBUG(EC_RSMRST_N, false);

    // Disable VDD5
    GPIO_SET_DEBUG(DD_ON, false);
    tPCH12;

#if HAVE_VA_EC_EN
    // Disable VCCPRIM_* planes
    GPIO_SET_DEBUG(VA_EC_EN, false);
#endif // HAVE_VA_EC_EN

#if HAVE_PCH_DPWROK_EC
    // De-assert DSW_PWROK
    GPIO_SET_DEBUG(PCH_DPWROK_EC, false);
#endif // HAVE_PCH_DPWROK_EC
    tPCH14;
#endif // DEEP_SX

    update_power_state();
}

// This function is run when the CPU is reset
void power_cpu_reset(void) {
    // LPC was just reset, enable PNP devices
    pnp_enable();
    // Reset ACPI registers
    acpi_reset();
    // Reset fans
    fan_reset();
    //TODO: reset KBC and touchpad states
    kbled_reset();
}

static bool power_button_disabled(void) {
    // Disable power button if lid is closed and AC is disconnected
    return !gpio_get(&LID_SW_N) && gpio_get(&ACIN_N);
}

void power_event(void) {
    // Always switch to ds5 if EC is running
    if (power_state == POWER_STATE_DEFAULT) {
        power_on_ds5();
    }

    // Check if the adapter line goes low
    static bool ac_send_sci = true;
    static bool ac_last = true;
    bool ac_new = gpio_get(&ACIN_N);
    if (ac_new != ac_last) {
        board_on_ac(!ac_new);

        DEBUG("Power adapter ");
        if (ac_new) {
            DEBUG("unplugged\n");
            battery_charger_disable();
        } else {
            DEBUG("plugged in\n");
            battery_charger_configure();
        }
        battery_debug();

        // Reset main loop cycle to force reading PECI and battery
        main_cycle = 0;

        // Send SCI to update AC and battery information
        ac_send_sci = true;
    }
    if (ac_send_sci) {
        // Send SCI 0x16 for AC detect event if ACPI OS is loaded
        if (acpi_ecos != EC_OS_NONE) {
            if (pmc_sci(&PMC_1, 0x16)) {
                ac_send_sci = false;
            }
        }
    }
    ac_last = ac_new;

    gpio_set(&AC_PRESENT, !ac_new);

    // Configure charger based on charging thresholds when plugged in
    if (!ac_new) {
        battery_charger_configure();
    }

    // Read power switch state
    static bool ps_last = true;
    bool ps_new = gpio_get(&PWR_SW_N);
    if (!ps_new && ps_last) {
        // Ensure press is not spurious
        for (int i = 0; i < 100; i++) {
            delay_ms(1);
            if (gpio_get(&PWR_SW_N) != ps_new) {
                DEBUG("%02X: Spurious press\n", main_cycle);
                ps_new = ps_last;
                break;
            } else if (power_button_disabled()) {
                // Ignore press when power button disabled
                ps_new = ps_last;
                break;
            }
        }

        if (ps_new != ps_last) {
            DEBUG("%02X: Power switch press\n", main_cycle);

            // Enable S5 power if necessary, before sending PWR_BTN
            update_power_state();
            if (power_state == POWER_STATE_DS5) {
                if (config_should_reset())
                    config_reset();
                power_on_s5();
            }
        }
    }
    #if LEVEL >= LEVEL_DEBUG
        else if (ps_new && !ps_last) {
            DEBUG("%02X: Power switch release\n", main_cycle);
        }
    #endif
    ps_last = ps_new;

    // Send power signal to PCH
    gpio_set(&PWR_BTN_N, ps_new);

    // Update power state before determining actions
    update_power_state();

    // If system power is good
    static bool pg_last = false;
    bool pg_new = gpio_get(&ALL_SYS_PWRGD);
    if (pg_new && !pg_last) {
        DEBUG("%02X: ALL_SYS_PWRGD asserted\n", main_cycle);

        //TODO: tPLT04;

        // Allow H_VR_READY to set PCH_PWROK
        GPIO_SET_DEBUG(PM_PWROK, true);

        // OEM defined delay from ALL_SYS_PWRGD to SYS_PWROK - TODO
        delay_ms(10);

#if HAVE_PCH_PWROK_EC
        // Assert SYS_PWROK, system can finally perform PLT_RST# and boot
        GPIO_SET_DEBUG(PCH_PWROK_EC, true);
#endif // HAVE_PCH_PWROK_EC
    } else if(!pg_new && pg_last) {
        DEBUG("%02X: ALL_SYS_PWRGD de-asserted\n", main_cycle);

#if HAVE_PCH_PWROK_EC
        // De-assert SYS_PWROK
        GPIO_SET_DEBUG(PCH_PWROK_EC, false);
#endif // HAVE_PCH_PWROK_EC

        // De-assert PCH_PWROK
        GPIO_SET_DEBUG(PM_PWROK, false);
    }
    pg_last = pg_new;

    static bool rst_last = false;
    bool rst_new = gpio_get(&BUF_PLT_RST_N);
    #if LEVEL >= LEVEL_DEBUG
        if (!rst_new && rst_last) {
            DEBUG("%02X: PLT_RST# asserted\n", main_cycle);
        } else
    #endif
    if(rst_new && !rst_last) {
        DEBUG("%02X: PLT_RST# de-asserted\n", main_cycle);
#if EC_ESPI
        espi_reset();
#else // EC_ESPI
        power_cpu_reset();
#endif // EC_ESPI
    }
    rst_last = rst_new;

#if HAVE_SLP_SUS_N
    #if LEVEL >= LEVEL_DEBUG
        static bool sus_last = true;
        bool sus_new = gpio_get(&SLP_SUS_N);
        if (!sus_new && sus_last) {
            DEBUG("%02X: SLP_SUS# asserted\n", main_cycle);
        } else if (sus_new && !sus_last) {
            DEBUG("%02X: SLP_SUS# de-asserted\n", main_cycle);
        }
        sus_last = sus_new;
    #endif
#endif // HAVE_SLP_SUS_N

#if EC_ESPI
    if (vw_get(&VW_SUS_PWRDN_ACK) == VWS_HIGH)
#elif HAVE_SUSWARN_N
    // EC must keep VccPRIM powered if SUSPWRDNACK is de-asserted low or system
    // state is S3
    static bool ack_last = false;
    bool ack_new = gpio_get(&SUSWARN_N);
    #if LEVEL >= LEVEL_DEBUG
        if (ack_new && !ack_last) {
            DEBUG("%02X: SUSPWRDNACK asserted\n", main_cycle);
        } else if (!ack_new && ack_last) {
            DEBUG("%02X: SUSPWRDNACK de-asserted\n", main_cycle);
        }
    #endif
    ack_last = ack_new;

    if (ack_new)
#endif // HAVE_SUSWARN_N
    {
        // Disable S5 power plane if not needed
        if (power_state == POWER_STATE_S5) {
            power_off_s5();
        }
    }

#if HAVE_LAN_WAKEUP_N
    static bool wake_last = true;
    bool wake_new = gpio_get(&LAN_WAKEUP_N);
    if (!wake_new && wake_last) {
        update_power_state();
        DEBUG("%02X: LAN_WAKEUP# asserted\n", main_cycle);
        if (power_state == POWER_STATE_DS5) {
            power_on_s5();
        }
    }
    #if LEVEL >= LEVEL_DEBUG
        else if (wake_new && !wake_last) {
            DEBUG("%02X: LAN_WAKEUP# de-asserted\n", main_cycle);
        }
    #endif
    wake_last = wake_new;
#endif // HAVE_LAN_WAKEUP_N

    static uint32_t last_time = 0;
    uint32_t time = time_get();
    if (power_state == POWER_STATE_S0) {
#if EC_ESPI
        if (!gpio_get(&CPU_C10_GATE_N)) {
            // Modern suspend, flashing green light
            if (
                (time < last_time) // overflow
                ||
                (time >= (last_time + 1000)) // timeout
            ) {
                gpio_set(&LED_PWR, !gpio_get(&LED_PWR));
                last_time = time;
            }
            gpio_set(&LED_ACIN, false);
        } else
#endif
        {
            // CPU on, green light
            gpio_set(&LED_PWR, true);
            gpio_set(&LED_ACIN, false);
        }
    } else if (power_state == POWER_STATE_S3 || power_state == POWER_STATE_DS3) {
        // Suspended, flashing green light
        if (
            (time < last_time) // overflow
            ||
            (time >= (last_time + 1000)) // timeout
        ) {
            gpio_set(&LED_PWR, !gpio_get(&LED_PWR));
            last_time = time;
        }
        gpio_set(&LED_ACIN, false);
    } else if (!ac_new) {
        // AC plugged in, orange light
        gpio_set(&LED_PWR, false);
        gpio_set(&LED_ACIN, true);
    } else {
        // CPU off and AC adapter unplugged, flashing orange light
        gpio_set(&LED_PWR, false);
        if (
            (time < last_time) // overflow
            ||
            (time >= (last_time + 1000)) // timeout
        ) {
            gpio_set(&LED_ACIN, !gpio_get(&LED_ACIN));
            last_time = time;
        }

#if HAVE_XLP_OUT
        // Power off VDD3 if system should be off
        gpio_set(&XLP_OUT, 0);
#endif // HAVE_XLP_OUT
    }

//TODO: do not require both LEDs
#if HAVE_LED_BAT_CHG && HAVE_LED_BAT_FULL
    if (!(battery_status & BATTERY_INITIALIZED)) {
        // No battery connected
        gpio_set(&LED_BAT_CHG, false);
        gpio_set(&LED_BAT_FULL, false);
    } else if (ac_new) {
        // Discharging (no AC adapter)
        gpio_set(&LED_BAT_CHG, false);
        gpio_set(&LED_BAT_FULL, false);
    } else if (battery_current == 0) {
        // Fully charged
        // TODO: turn off charger
        gpio_set(&LED_BAT_CHG, false);
        gpio_set(&LED_BAT_FULL, true);
    } else {
        // Charging
        gpio_set(&LED_BAT_CHG, true);
        gpio_set(&LED_BAT_FULL, false);
    }
#endif // HAVE_LED_BAT_CHG && HAVE_LED_BAT_FULL
}
