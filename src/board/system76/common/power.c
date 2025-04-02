// SPDX-License-Identifier: GPL-3.0-only

#include <arch/delay.h>
#include <arch/time.h>
#include <board/acpi.h>
#include <board/battery.h>
#include <board/board.h>
#include <board/config.h>
#include <board/fan.h>
#include <board/gpio.h>
#include <board/kbc.h>
#include <board/kbled.h>
#include <board/lid.h>
#include <board/options.h>
#include <board/peci.h>
#include <board/power.h>
#include <board/pmc.h>
#include <board/pnp.h>
#include <board/ps2.h>
#include <board/usbpd.h>
#include <board/wireless.h>
#include <common/debug.h>
#include <board/dgpu.h>

#if CONFIG_BUS_ESPI
#include <ec/espi.h>
#include <board/espi.h>
#endif

#if CONFIG_SECURITY
#include <board/security.h>
#endif // CONFIG_SECURITY

#define GPIO_SET_DEBUG(G, V) \
    { \
        DEBUG("%s = %s\n", #G, V ? "true" : "false"); \
        gpio_set(&G, V); \
    }

#ifndef USE_S0IX
#define USE_S0IX 0
#endif

#ifndef POWER_LIMIT_PSYS_ENABLE
#define POWER_LIMIT_PSYS_ENABLE 1
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

#ifndef HAVE_PM_PWROK
#define HAVE_PM_PWROK 1
#endif

#ifndef HAVE_SLP_SUS_N
#define HAVE_SLP_SUS_N 1
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

// Only galp6 has this, so disable by default.
#ifndef HAVE_PD_EN
#define HAVE_PD_EN 0
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
#define tPCH18 delay_ms(95)
// DSW_PWROK assertion to SLP_SUS# de-assertion
#define tPCH32 delay_ms(95)
// RSMRST# de-assertion to SUSPWRDNACK valid
#define tPLT01 delay_ms(200)

enum PowerState power_state = POWER_STATE_G3;

#if USE_S0IX
uint8_t pep_hook = PEP_DISPLAY_FLAG;
#endif

enum PowerState calculate_power_state(void) {
    if (!gpio_get(&USB_CHARGE_EN)) {
        // VDD5, VccPRIM not powered
        return POWER_STATE_G3;
    }

    if (!gpio_get(&EC_RSMRST_N)) {
        // S5 plane not powered
        return POWER_STATE_G3_AOU;
    }

#if CONFIG_BUS_ESPI
    // Use eSPI virtual wires if available

    if (vw_get(&VW_SLP_S4_N) != VWS_HIGH) {
        // S4 plane not powered
        return POWER_STATE_S5;
    }

    if (vw_get(&VW_SLP_S3_N) != VWS_HIGH) {
        // S3 plane not powered
        return POWER_STATE_S3;
    }
#else // CONFIG_BUS_ESPI
    // Use dedicated GPIOs if not using ESPI

    if (!gpio_get(&SUSC_N_PCH)) {
        // S4 plane not powered
        return POWER_STATE_S5;
    }

    if (!gpio_get(&SUSB_N_PCH)) {
        // S3 plane not powered
        return POWER_STATE_S3;
    }
#endif // CONFIG_BUS_ESPI

    // All planes are powered
    return POWER_STATE_S0;
}

void update_power_state(void) {
    enum PowerState new_power_state = calculate_power_state();
    if (power_state != new_power_state) {
        power_state = new_power_state;

        if (power_state != POWER_STATE_S0)
            pep_hook = PEP_DISPLAY_FLAG;

#if LEVEL >= LEVEL_DEBUG
        switch (power_state) {
        case POWER_STATE_G3:
            DEBUG("POWER_STATE_G3\n");
            break;
        case POWER_STATE_G3_AOU:
            DEBUG("POWER_STATE_G3_AOU\n");
            break;
        case POWER_STATE_S5:
            DEBUG("POWER_STATE_S5\n");
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

static bool is_standby_power_needed(void) {
    if (options_get(OPT_ALWAYS_ON_USB))
        return true;

    return false;
}

void power_init(void) {
    // See Figure 12-19 in Whiskey Lake Platform Design Guide
    // | VCCRTC | RTCRST# | VccPRIM |
    // | tPCH01---------- |         |
    // | tPCH04-------------------- |

    // tPCH04 is the ideal delay
    tPCH04;

    update_power_state();
}

static void power_sequence_g3_g3aou(void) {
#if HAVE_VA_EC_EN
    // Enable VCCPRIM_* planes - must be enabled prior to USB power in order to
    // avoid leakage
    GPIO_SET_DEBUG(VA_EC_EN, true);
#endif // HAVE_VA_EC_EN
#if HAVE_PD_EN
    GPIO_SET_DEBUG(PD_EN, true);
#endif
    tPCH06;
    // Enable VDD5
    GPIO_SET_DEBUG(USB_CHARGE_EN, true);
}

static void power_sequence_g3aou_s5(void) {
    // Configure WLAN GPIOs before powering on
    wireless_power(true);

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
}

static void power_sequence_s5_s0(void) {
    GPIO_SET_DEBUG(PWR_BTN_N, false);
    delay_ms(32); // PWRBTN# must assert for at least 16 ms, we do twice that
    GPIO_SET_DEBUG(PWR_BTN_N, true);

    uint16_t i;
    for (i = 0; i < 5000; i++) {
        // If we reached S0, exit this loop
        update_power_state();
        if (power_state == POWER_STATE_S0) {
            DEBUG("reached S0 in %d ms\n", i);
            break;
        }

#if CONFIG_BUS_ESPI
        // Check for VW changes
        espi_event();
#endif // CONFIG_BUS_ESPI

        // Extra wait until SUSPWRDNACK is valid
        delay_ms(1);
    }

    if (power_state != POWER_STATE_S0) {
        DEBUG("failed to reach S0, powering off\n");
        power_off();
    } else {
        kbc_clear_lock();
        ps2_reset(&PS2_1);
        ps2_reset(&PS2_TOUCHPAD);
    }
}

static void power_sequence_sx_g3aou(void) {
#if HAVE_PCH_PWROK_EC
    // De-assert SYS_PWROK
    GPIO_SET_DEBUG(PCH_PWROK_EC, false);
#endif // HAVE_PCH_PWROK_EC

#if HAVE_PM_PWROK
    // De-assert PCH_PWROK
    GPIO_SET_DEBUG(PM_PWROK, false);
#endif // HAVE_PM_PWROK

#if HAVE_EC_EN
    // Block processor from controlling SUSB# and SUSC#
    GPIO_SET_DEBUG(EC_EN, false);
#endif // HAVE_EC_EN

    // De-assert RSMRST#
    GPIO_SET_DEBUG(EC_RSMRST_N, false);

#if HAVE_PCH_DPWROK_EC
    // De-assert DSW_PWROK
    GPIO_SET_DEBUG(PCH_DPWROK_EC, false);
#endif // HAVE_PCH_DPWROK_EC
    tPCH14;

    // Configure WLAN GPIOs after powering off
    wireless_power(false);

    GPIO_SET_DEBUG(DD_ON, false);
}

static void power_sequence_g3aou_g3(void) {
    // Disable VDD5
    GPIO_SET_DEBUG(USB_CHARGE_EN, false);
    tPCH12;

#if HAVE_PD_EN
    GPIO_SET_DEBUG(PD_EN, false);
#endif
#if HAVE_VA_EC_EN
    // Disable VCCPRIM_* planes
    GPIO_SET_DEBUG(VA_EC_EN, false);
#endif // HAVE_VA_EC_EN
}

static void power_sequence(enum PowerState target) {
    update_power_state();

    if (target == power_state) {
        return;
    }

    DEBUG("Power state: %x -> %x\n", power_state, target);
    if (target < power_state) {
        if (power_state == POWER_STATE_G3)
            power_sequence_g3_g3aou();

        if (target == POWER_STATE_G3_AOU)
            return;

        if (power_state > POWER_STATE_S5)
            power_sequence_g3aou_s5();

        if (target == POWER_STATE_S5)
            return;

        if (power_state > POWER_STATE_S0)
            power_sequence_s5_s0();
    }

    if (target > power_state) {
        if (target < POWER_STATE_G3_AOU) {
            WARN("Warning: S3 or S5 entry from S0 must be initiated by AP!\n");
            return;
        }

        if (power_state < POWER_STATE_G3_AOU)
            power_sequence_sx_g3aou();

        if (target == POWER_STATE_G3_AOU)
            return;

        if (power_state < POWER_STATE_G3)
            power_sequence_g3aou_g3();
    }

    update_power_state();
}

void power_on(void) {
    DEBUG("%02X: power_on\n", main_cycle);

    power_sequence(POWER_STATE_S0);
}

void power_off(void) {
    DEBUG("%02X: power_off\n", main_cycle);

    power_sequence(is_standby_power_needed() ? POWER_STATE_G3_AOU : POWER_STATE_G3);
}

void power_apply_limit(bool ac) {
    uint32_t supply_watts = 0;
    uint32_t watts = 0;
    uint16_t res;

    if (!peci_available()) {
        DEBUG("PECI not yet available, skip PL\n");
    } else {
        if (ac) {
            supply_watts = (uint32_t)battery_charger_input_current_ma *
                (uint32_t)battery_charger_input_voltage_v / 1000;
            DEBUG("%llu W supply detected\n", supply_watts);
        }

        if (!ac || !supply_watts || options_get(OPT_ALLOW_BAT_BOOST)) {
            supply_watts += POWER_LIMIT_DC;
        }

        DEBUG(
            "PECI PL: %llu watts available from %s\n",
            supply_watts,
            ac ? options_get(OPT_ALLOW_BAT_BOOST) ? "AC + DC" : "AC" : "DC"
        );

#if POWER_LIMIT_PSYS_ENABLE
        // Set PsysL2 to 2W below supply wattage (Intel says PL3 = PL2 + 2W)
        watts = supply_watts - 2;
        res = peci_wr_pkg_config(PECI_REG_PKG_CFG_PSYS_PL2, 0, PECI_PSYS_PL2(watts));
        DEBUG(" SET PsysPL2 = %llu %s\n", watts, (res == 0x40) ? "OK" : "ERR");

        // Set PsysPL3 (peak short term power) to supply wattage
        watts = supply_watts;
        res = peci_wr_pkg_config(PECI_REG_PKG_CFG_PL3, 0, PECI_PL3(watts, 6, 4));
        DEBUG(" SET PL3 = %llu %s\n", watts, (res == 0x40) ? "OK" : "ERR");
#endif

        // Cap PL4 to supply wattage if needed
        watts = (supply_watts > POWER_LIMIT_AC) ? POWER_LIMIT_AC : supply_watts;
        res = peci_wr_pkg_config(PECI_REG_PKG_CFG_PL4, 0, PECI_PL4(watts));
        DEBUG(" SET PL4 = %llu %s\n", watts, (res == 0x40) ? "OK" : "ERR");
    }

#if HAVE_D_NOTIFY
    // Send D-notification for GPU to re-evaluate power limit
    pmc_sci(&PMC_1, 0xA0);
#endif

    return;
}

// This function is run when the CPU is reset
void power_cpu_reset(void) {
#if HAVE_DGPU && HAVE_MUX_CTRL_BIOS
    // Set GPIO MUX_CTRL_BIOS to choose between iGPU and dGPU
    set_mux_ctrl();
#endif //HAVE_DGPU
    // LPC was just reset, enable PNP devices
    pnp_enable();
    // Reset ACPI registers
    acpi_reset();
    // Reset fans
    fan_reset();
    // Reset KBC and touchpad states
    kbled_reset();
    kbc_clear_lock();
    ps2_reset(&PS2_1);
    ps2_reset(&PS2_TOUCHPAD);
    // Set power limits
    power_apply_limit(!gpio_get(&ACIN_N));

    // Read Tjunction
    uint32_t pkg_temp_target;
    if (!peci_rd_pkg_config(16, 0, &pkg_temp_target))
        t_junction = (pkg_temp_target >> 16) & 0xFF;
}

static bool power_button_disabled(void) {
    // Disable power button if lid is closed and AC is disconnected
    return !gpio_get(&LID_SW_N) && gpio_get(&ACIN_N);
}

void power_event(void) {
    // Check if the adapter line goes low
    static bool ac_send_sci = true;
    static bool ac_last = true;
    static uint32_t ac_unplug_time = 0;
    bool ac_new = gpio_get(&ACIN_N);

    if (power_state == POWER_STATE_G3 && is_standby_power_needed())
        power_sequence(POWER_STATE_G3_AOU);

    if (ac_new != ac_last) {
        // Configure smart charger
        DEBUG("Power adapter ");
        if (ac_new) {
            DEBUG("unplugged\n");
            GPIO_SET_DEBUG(H_PROCHOT_EC, false);
            ac_unplug_time = time_get();
            battery_charger_disable();
        } else {
            DEBUG("plugged in\n");
            battery_charger_configure();
            if (options_get(OPT_POWER_ON_AC) == 1)
                power_on();
        }
        power_apply_limit(!ac_new);
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

    // Unthrottle on AC, or after 3 seconds on DC
    gpio_set(&H_PROCHOT_EC, !ac_new | (ac_unplug_time < (time_get() - 3000)));

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
        for (uint8_t i = 100; i != 0; i--) {
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
            if (power_state >= POWER_STATE_G3_AOU) {
                if (config_should_reset())
                    config_reset();
                power_on();

                // After power on ensure there is no secondary press sent to PCH
                ps_new = ps_last;
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

#if HAVE_PM_PWROK
        // Allow H_VR_READY to set PCH_PWROK
        GPIO_SET_DEBUG(PM_PWROK, true);
#endif // HAVE_PM_PWROK

        // OEM defined delay from ALL_SYS_PWRGD to SYS_PWROK - TODO
        delay_ms(10);

#if HAVE_PCH_PWROK_EC
        // Assert SYS_PWROK, system can finally perform PLT_RST# and boot
        GPIO_SET_DEBUG(PCH_PWROK_EC, true);
#endif // HAVE_PCH_PWROK_EC
    } else if (!pg_new && pg_last) {
        DEBUG("%02X: ALL_SYS_PWRGD de-asserted\n", main_cycle);

#if HAVE_PCH_PWROK_EC
        // De-assert SYS_PWROK
        GPIO_SET_DEBUG(PCH_PWROK_EC, false);
#endif // HAVE_PCH_PWROK_EC

#if HAVE_PM_PWROK
        // De-assert PCH_PWROK
        GPIO_SET_DEBUG(PM_PWROK, false);
#endif // HAVE_PM_PWROK
    }
    pg_last = pg_new;

    // clang-format off
    static bool rst_last = false;
    bool rst_new = gpio_get(&BUF_PLT_RST_N);
#if LEVEL >= LEVEL_DEBUG
    if (!rst_new && rst_last) {
        DEBUG("%02X: PLT_RST# asserted\n", main_cycle);
    } else
#endif
    if (rst_new && !rst_last) {
        DEBUG("%02X: PLT_RST# de-asserted\n", main_cycle);
#if CONFIG_BUS_ESPI
        espi_reset();
#else // CONFIG_BUS_ESPI
        power_cpu_reset();
#endif // CONFIG_BUS_ESPI
    }
    rst_last = rst_new;
    // clang-format on

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

#if CONFIG_BUS_ESPI
    // ESPI systems must keep S5 planes powered unless VW_SUS_PWRDN_ACK is high
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
            power_off();

#if CONFIG_SECURITY
            // Handle security state changes if necessary
            if (security_power()) {
                power_on();
            }
#endif // CONFIG_SECURITY
        }
    }

#if HAVE_LAN_WAKEUP_N
    static bool wake_last = true;
    bool wake_new = gpio_get(&LAN_WAKEUP_N);
    if (!wake_new && wake_last) {
        update_power_state();
        DEBUG("%02X: LAN_WAKEUP# asserted\n", main_cycle);
        if (power_state == POWER_STATE_G3) {
            power_on();
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
#if USE_S0IX
        if (pep_hook & PEP_S0IX_FLAG) {
            // Modern suspend, flashing green light
            if ((time - last_time) >= 1000) {
                gpio_set(&LED_PWR, !gpio_get(&LED_PWR));
                last_time = time;
            }
            kbled_enable(false);
            gpio_set(&LED_ACIN, false);
        } else
#endif
        {
            // CPU on, green light
            gpio_set(&LED_PWR, true);
            gpio_set(&LED_ACIN, false);

            if (gpio_get(&LID_SW_N) && (pep_hook & PEP_DISPLAY_FLAG))
                kbled_enable(true);
            else
                kbled_enable(false);
        }
    } else if (power_state == POWER_STATE_S3) {
        // Suspended, flashing green light
        if ((time - last_time) >= 1000) {
            gpio_set(&LED_PWR, !gpio_get(&LED_PWR));
            last_time = time;
        }
        gpio_set(&LED_ACIN, false);
        kbled_enable(false);
    } else if (!ac_new) {
        // AC plugged in, orange light
        gpio_set(&LED_PWR, false);
        gpio_set(&LED_ACIN, true);
    } else if (power_state == POWER_STATE_G3_AOU) {
        // G3, Always on USB, lights off
        gpio_set(&LED_PWR, false);
        gpio_set(&LED_ACIN, false);
    } else {
        // CPU off and AC adapter unplugged, flashing orange light
        gpio_set(&LED_PWR, false);
        if ((time - last_time) >= 1000) {
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
    if (!(battery_info.status & BATTERY_INITIALIZED)) {
        // No battery connected
        gpio_set(&LED_BAT_CHG, false);
        gpio_set(&LED_BAT_FULL, false);
    } else if (ac_new) {
        // Discharging (no AC adapter)
        gpio_set(&LED_BAT_CHG, false);
        gpio_set(&LED_BAT_FULL, false);
    } else if (battery_info.charge >= battery_get_start_threshold()) {
        // Fully charged
        gpio_set(&LED_BAT_CHG, false);
        gpio_set(&LED_BAT_FULL, true);
    } else {
        // Charging
        gpio_set(&LED_BAT_CHG, true);
        gpio_set(&LED_BAT_FULL, false);
    }
#endif // HAVE_LED_BAT_CHG && HAVE_LED_BAT_FULL
}
