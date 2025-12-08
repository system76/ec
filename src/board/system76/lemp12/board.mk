# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2023 System76, Inc.

board-y += board.c
board-y += gpio.c

EC = ite
CONFIG_EC_ITE_IT5570E = y
CONFIG_EC_FLASH_SIZE_128K = y

# Intel-based host
CONFIG_PLATFORM_INTEL = y
CONFIG_BUS_ESPI = y

# Enable firmware security
CONFIG_SECURITY = y

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
#TODO: why is psys gain 0.5uA/W
CONFIG_CHARGER = oz26786
CONFIG_CHARGER_ADAPTER_RSENSE = 5
CONFIG_CHARGER_BATTERY_RSENSE = 5
CONFIG_CHARGER_CHARGE_CURRENT = 3072
CONFIG_CHARGER_CHARGE_VOLTAGE = 8800
CONFIG_CHARGER_INPUT_CURRENT = 3420
CONFIG_CHARGER_PSYS_GAIN = 500

# Set CPU power limits in watts
CONFIG_POWER_LIMIT_AC = 65
CONFIG_POWER_LIMIT_DC = 45

# Fan configs
CONFIG_FAN1_PWM = DCR2
CONFIG_FAN1_POINTS = " \
	FAN_POINT(70, 40), \
	FAN_POINT(75, 50), \
	FAN_POINT(80, 60), \
	FAN_POINT(85, 65), \
	FAN_POINT(90, 65), \
"

# Add system76 common code
include src/board/system76/common/common.mk
