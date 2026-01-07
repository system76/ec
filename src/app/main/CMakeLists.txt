# SPDX-License-Identifier: GPL-3.0-only

target_sources(ec PRIVATE
    acpi.c
    battery.c
    config.c
    ecpm.c
    fan.c
    gctrl.c
    kbc.c
    kbscan.c
    keymap.c
    lid.c
    main.c
    parallel.c
    pmc.c
    pnp.c
    ps2.c
    pwm.c
    scratch.c
    smbus.c
    smfi.c
    stdio.c
    wireless.c
)

target_include_directories(ec PUBLIC include)

target_compile_options(ec PRIVATE
    -mmcs51
    --model-large
    "--code-size ${CODE_SIZE}"
    --xram-size=${SRAM_SIZE}
    --Werror
)

target_link_options(ec PRIVATE
    -mmcs51
    --model-large
    "--code-size ${CODE_SIZE}"
    --xram-size=${SRAM_SIZE}
    --Werror
)

# Set log level
# 0 - NONE
# 1 - ERROR
# 2 - WARN
# 3 - INFO
# 4 - DEBUG
# 5 - TRACE
target_compile_definitions(ec PUBLIC LEVEL=4)

# Uncomment to enable debug logging over keyboard parallel port
#target_compile_definitions(ec PUBLIC PARALLEL_DEBUG)

target_compile_definitions(ec PUBLIC I2C_SMBUS=${CONFIG_I2C_SMBUS})
# Uncomment to enable I2C debug on 0x76
#target_compile_definitions(ec PUBLIC I2C_DEBUGGER=0x76)

if(CONFIG_SECURITY)
    target_sources(ec PRIVATE security.c)
    target_compile_definitions(ec PUBLIC CONFIG_SECURITY=1)
endif()

if(CONFIG_PLATFORM_INTEL)
    target_sources(ec PRIVATE
        peci.c
        power/intel.c
    )
    target_compile_definitions(ec PUBLIC CONFIG_PLATFORM_INTEL=1)
elseif(CONFIG_PLATFORM_AMD)
    target_sources(ec PRIVATE power/amd.c)
    target_compile_definitions(ec PUBLIC CONFIG_PLATFORM_AMD=1)
else()
    message(FATAL_ERROR "PLATFORM not specified")
endif()

# Set CPU power limits
target_compile_definitions(ec PRIVATE
    POWER_LIMIT_AC=${CONFIG_POWER_LIMIT_AC}
    POWER_LIMIT_DC=${CONFIG_POWER_LIMIT_DC}
)

if(CONFIG_BUS_ESPI)
    target_sources(ec PRIVATE espi.c)
    target_compile_definitions(ec PUBLIC CONFIG_BUS_ESPI=1)

    # TODO: Use PECI over eSPI on all boards using eSPI
    if(CONFIG_PECI_OVER_ESPI)
        target_compile_definitions(ec PUBLIC CONFIG_PECI_OVER_ESPI=1)
    endif()
endif()

if(CONFIG_HAVE_DGPU)
    target_sources(ec PRIVATE dgpu.c)
    target_compile_definitions(ec PUBLIC
        CONFIG_HAVE_DGPU=1
        I2C_DGPU=${CONFIG_I2C_DGPU}
    )
endif()

# Fan configs
if(DEFINED CONFIG_FAN1_PWM)
    target_compile_definitions(ec PUBLIC FAN1_PWM=${CONFIG_FAN1_PWM})
    if(DEFINED CONFIG_FAN1_PWM_MIN)
        target_compile_definitions(ec PUBLIC FAN1_PWM_MIN=${CONFIG_FAN1_PWM_MIN})
    endif()
    target_compile_definitions(ec PUBLIC BOARD_FAN1_POINTS=${CONFIG_FAN1_POINTS})
endif()

if(DEFINED CONFIG_FAN2_PWM)
    target_compile_definitions(ec PUBLIC FAN2_PWM=${CONFIG_FAN2_PWM})
    if(DEFINED CONFIG_FAN2_PWM_MIN)
        target_compile_definitions(ec PUBLIC FAN2_PWM_MIN=${CONFIG_FAN2_PWM_MIN})
    endif()
    target_compile_definitions(ec PUBLIC BOARD_FAN2_POINTS=${CONFIG_FAN2_POINTS})
endif()

# Set battery charging thresholds
target_compile_definitions(ec PUBLIC
    BATTERY_START_THRESHOLD=90
    BATTERY_END_THRESHOLD=100
)

# Add charger
if(DEFINED CONFIG_CHARGER)
    target_sources(ec PRIVATE charger/${CONFIG_CHARGER}.c)
    target_compile_definitions(ec PUBLIC
        CHARGER_ADAPTER_RSENSE=${CONFIG_CHARGER_ADAPTER_RSENSE}
        CHARGER_BATTERY_RSENSE=${CONFIG_CHARGER_BATTERY_RSENSE}
        CHARGER_CHARGE_CURRENT=${CONFIG_CHARGER_CHARGE_CURRENT}
        CHARGER_CHARGE_VOLTAGE=${CONFIG_CHARGER_CHARGE_VOLTAGE}
        CHARGER_INPUT_CURRENT=${CONFIG_CHARGER_INPUT_CURRENT}
    )
    if(DEFINED CONFIG_CHARGER_PSYS_GAIN)
        target_compile_definitions(ec PUBLIC CHARGER_PSYS_GAIN=${CONFIG_CHARGER_PSYS_GAIN})
    endif()
endif()

# Add USB-PD
if(CONFIG_HAVE_USBPD)
    target_compile_definitions(ec PUBLIC CONFIG_HAVE_USBPD=1)
    target_compile_definitions(ec PUBLIC I2C_USBPD=${CONFIG_I2C_USBPD})
    if(CONFIG_USBPD_TPS65987)
        target_sources(ec PRIVATE usbpd/tps65987.c)
    endif()
endif()

# Add keyboard
if(NOT DEFINED KEYBOARD)
    message(FATAL_ERROR "KEYBOARD is not set by the board")
endif()

# Add kbled
if(CONFIG_HAVE_KBLED)
    target_sources(ec PRIVATE
        kbled/common.c
        kbled/${CONFIG_KBLED}.c
    )
    target_compile_definitions(ec PUBLIC CONFIG_HAVE_KBLED=1)
    if(DEFINED CONFIG_KBLED_DAC)
        target_compile_definitions(ec PUBLIC KBLED_DAC=${CONFIG_KBLED_DAC})
    endif()
    if(DEFINED CONFIG_KBLED_I2C)
        target_compile_definitions(ec PUBLIC KBLED_I2C=${CONFIG_KBLED_I2C})
        if(CONFIG_KBLED_I2C_CH_BGR_PACKED)
            target_compile_definitions(ec PUBLIC CONFIG_KBLED_I2C_CH_BGR_PACKED=1)
        elseif(CONFIG_KBLED_I2C_CH_BGR_MIXED)
            target_compile_definitions(ec PUBLIC CONFIG_KBLED_I2C_CH_BGR_MIXED=1)
        endif()
    endif()
endif()

target_compile_definitions(ec PUBLIC PS2_TOUCHPAD=${CONFIG_PS2_TOUCHPAD})

# Add scratch ROM
add_subdirectory(scratch)

# Add scratch ROM for flash access
add_subdirectory(flash)
target_sources(ec PRIVATE flash/wrapper.c)
