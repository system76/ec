# SPDX-License-Identifier: GPL-3.0-only

board-y += board.c
board-y += gpio.c

EC = ite
CONFIG_EC_ITE_IT5570E = y
CONFIG_EC_FLASH_SIZE_256K = y

# Enable eSPI
CONFIG_BUS_ESPI = y
CONFIG_PECI_OVER_ESPI = y

# Enable firmware security
CONFIG_SECURITY = y

# Set keyboard configs
KEYBOARD = 18H9LHA04

CONFIG_HAVE_KBLED = y
KBLED = rgb_pwm

# Set touchpad PS2 bus
CFLAGS += -DPS2_TOUCHPAD=PS2_3

# Set smart charger parameters
CHARGER = oz26786
CFLAGS += -DI2C_SMBUS=I2C_4
CFLAGS += \
	-DCHARGER_ADAPTER_RSENSE=5 \
	-DCHARGER_BATTERY_RSENSE=10 \
	-DCHARGER_CHARGE_CURRENT=3072 \
	-DCHARGER_CHARGE_VOLTAGE=17400 \
	-DCHARGER_INPUT_CURRENT=11500

# Set CPU power limits in watts
CFLAGS += \
	-DPOWER_LIMIT_AC=230 \
	-DPOWER_LIMIT_DC=45

# Enable DGPU support
CONFIG_HAVE_DGPU = y
CFLAGS += -DI2C_DGPU=I2C_1

# Fan configs
CFLAGS += -DFAN1_PWM=DCR2
CFLAGS += -DFAN1_TABLE="\
	FAN_LEVEL(60, 0, 0), \
	FAN_LEVEL(60, 55, 28), \
	FAN_LEVEL(65, 55, 28), \
	FAN_LEVEL(70, 65, 40), \
	FAN_LEVEL(75, 70, 60), \
	FAN_LEVEL(80, 75, 75), \
	FAN_LEVEL(85, 80, 90), \
	FAN_LEVEL(90, 85, 100), \
"

CFLAGS += -DFAN2_PWM=DCR4
CFLAGS += -DFAN2_TABLE="\
	FAN_LEVEL(60, 0, 0), \
	FAN_LEVEL(60, 55, 28), \
	FAN_LEVEL(65, 55, 28), \
	FAN_LEVEL(70, 65, 40), \
	FAN_LEVEL(75, 70, 60), \
	FAN_LEVEL(80, 75, 75), \
	FAN_LEVEL(85, 80, 90), \
	FAN_LEVEL(90, 85, 100), \
"

# Add system76 common code
include src/board/system76/common/common.mk
