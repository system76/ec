# SPDX-License-Identifier: GPL-3.0-only

board-y += ../bonw15/board.c
board-y += ../bonw15/gpio.c

# FIXME: Handle this better
CFLAGS += -I$(BOARD_DIR)/../bonw15/include

EC = ite
CONFIG_EC_ITE_IT5570E = y
CONFIG_EC_FLASH_SIZE_256K = y

# Intel-based host
CONFIG_PLATFORM_INTEL = y
CONFIG_BUS_ESPI = y

# Enable firmware security
CONFIG_SECURITY = y

# Include keyboard
KEYBOARD = 15in_102

# Set keyboard LED mechanism
CONFIG_HAVE_KBLED = y
CONFIG_KBLED = bonw14

# Set battery I2C bus
CONFIG_I2C_SMBUS = I2C_4

# Set touchpad PS2 bus
CONFIG_PS2_TOUCHPAD = PS2_3

# Set smart charger parameters
# XXX: PRS1 and PRS2 are in parallel for adapter Rsense?
CONFIG_CHARGER = oz26786
CONFIG_CHARGER_ADAPTER_RSENSE = 5
CONFIG_CHARGER_BATTERY_RSENSE = 5
CONFIG_CHARGER_CHARGE_CURRENT = 3072
CONFIG_CHARGER_CHARGE_VOLTAGE = 17400
CONFIG_CHARGER_INPUT_CURRENT = 16920

# Set USB-PD parameters
CONFIG_DRIVERS_USBPD_TPS65987 = y
CONFIG_USBPD_I2C = 1

# Set CPU power limits in watts
CONFIG_POWER_LIMIT_AC = 330
CONFIG_POWER_LIMIT_DC = 55

# Enable dGPU support
CONFIG_DRIVERS_DGPU_NVIDIA = y
CONFIG_DGPU_I2C = 1

# Fan configs
CONFIG_FAN1_PWM = DCR2
CONFIG_FAN1_POINTS = " \
	FAN_POINT(60, 40), \
	FAN_POINT(65, 60), \
	FAN_POINT(70, 75), \
	FAN_POINT(75, 90), \
	FAN_POINT(80, 100), \
"

CONFIG_FAN2_PWM = DCR4
CONFIG_FAN2_POINTS = " \
	FAN_POINT(60, 40), \
	FAN_POINT(65, 60), \
	FAN_POINT(70, 75), \
	FAN_POINT(75, 90), \
	FAN_POINT(80, 100), \
"

# Add system76 common code
include src/board/system76/common/common.mk
