# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2020 System76, Inc.
# SPDX-FileCopyrightText: 2021 Winston Hoy

board-y += board.c
board-y += gpio.c

EC = ite
CONFIG_EC_ITE_IT5570E = y
CONFIG_EC_FLASH_SIZE_128K = y

# Intel-based host
CONFIG_PLATFORM_INTEL = y
CONFIG_BUS_ESPI = y

# Include keyboard
KEYBOARD = 14in_83

# Set keyboard LED mechanism
CONFIG_HAVE_KBLED = y
CONFIG_KBLED = white_dac
CONFIG_KBLED_DAC = 2

# Set battery I2C bus
CONFIG_I2C_SMBUS = I2C_4

# Set touchpad PS2 bus
CONFIG_PS2_TOUCHPAD = PS2_3

# Set smart charger parameters
# FIXME: Verify parts and values.
CONFIG_CHARGER = oz26786
CONFIG_CHARGER_ADAPTER_RSENSE = 10
CONFIG_CHARGER_BATTERY_RSENSE = 10
CONFIG_CHARGER_CHARGE_CURRENT = 1536
CONFIG_CHARGER_CHARGE_VOLTAGE = 17400
CONFIG_CHARGER_INPUT_CURRENT = 3420

# Set CPU power limits in watts
CONFIG_POWER_LIMIT_AC = 65
CONFIG_POWER_LIMIT_DC = 45

# Enable dGPU support
CONFIG_HAVE_DGPU = y
CONFIG_I2C_DGPU = I2C_1

# Fan configs
CONFIG_FAN1_PWM = DCR2
CONFIG_FAN1_POINTS = " \
	FAN_POINT(70, 25), \
	FAN_POINT(80, 25), \
	FAN_POINT(80, 40), \
	FAN_POINT(88, 40), \
	FAN_POINT(88, 100), \
"

CONFIG_FAN2_PWM = DCR4
CONFIG_FAN2_POINTS = " \
	FAN_POINT(70, 25), \
	FAN_POINT(75, 40), \
	FAN_POINT(80, 75), \
	FAN_POINT(85, 90), \
	FAN_POINT(90, 100), \
"

# Add system76 common code
include src/board/system76/common/common.mk
