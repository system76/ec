# SPDX-License-Identifier: GPL-3.0-only

board-y += board.c
board-y += gpio.c

EC = ite
CONFIG_EC_ITE_IT5570E = y
CONFIG_EC_FLASH_SIZE_256K = y

# Enable eSPI
CONFIG_BUS_ESPI = y

# Enable firmware security
CONFIG_SECURITY = y

# Keyboard configuration
KEYBOARD = 18H9LHA05
KEYMAP = darp10
CONFIG_HAVE_KBLED = y
KBLED = rgb_pwm

# Set battery I2C bus
CFLAGS += -DI2C_SMBUS=I2C_4

# Set touchpad PS2 bus
CFLAGS += -DPS2_TOUCHPAD=PS2_3

# Set smart charger parameters
CHARGER = oz26786
CFLAGS += \
	-DCHARGER_ADAPTER_RSENSE=5 \
	-DCHARGER_BATTERY_RSENSE=10 \
	-DCHARGER_CHARGE_CURRENT=1536 \
	-DCHARGER_CHARGE_VOLTAGE=17600 \
	-DCHARGER_INPUT_CURRENT=4740

# Set CPU power limits in watts
CFLAGS += \
	-DPOWER_LIMIT_AC=65 \
	-DPOWER_LIMIT_DC=45

# Fan configs
CFLAGS += -DFAN1_PWM=DCR2
CFLAGS += -DBOARD_FAN1_POINTS="\
	FAN_POINT(70, 40), \
	FAN_POINT(75, 50), \
	FAN_POINT(80, 60), \
	FAN_POINT(85, 65), \
	FAN_POINT(90, 65), \
"

CFLAGS += -DFAN2_PWM=DCR3
CFLAGS += -DBOARD_FAN2_POINTS="\
	FAN_POINT(70, 40), \
	FAN_POINT(75, 50), \
	FAN_POINT(80, 60), \
	FAN_POINT(85, 65), \
	FAN_POINT(90, 65), \
"

# Add common code
include src/board/system76/common/common.mk
