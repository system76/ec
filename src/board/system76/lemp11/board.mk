# SPDX-License-Identifier: GPL-3.0-only

board-y += board.c
board-y += gpio.c

EC=ite
CONFIG_EC_ITE_IT5570E=y
CONFIG_EC_FLASH_SIZE_128K = y

# Enable eSPI
CONFIG_BUS_ESPI=y

# Include keyboard
KEYBOARD=14in_83

# Set keyboard LED mechanism
CONFIG_HAVE_KBLED = y
KBLED=white_dac
CFLAGS+=-DKBLED_DAC=2

# Set battery I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_4

# Set touchpad PS2 bus
CFLAGS+=-DPS2_TOUCHPAD=PS2_3

# Set smart charger parameters
CHARGER=oz26786
CFLAGS+=\
	-DCHARGER_ADAPTER_RSENSE=5 \
	-DCHARGER_BATTERY_RSENSE=5 \
	-DCHARGER_CHARGE_CURRENT=3072 \
	-DCHARGER_CHARGE_VOLTAGE=8800 \
	-DCHARGER_INPUT_CURRENT=3420

# Set CPU power limits in watts
CFLAGS+=\
	-DPOWER_LIMIT_AC=65 \
	-DPOWER_LIMIT_DC=45

# Fan configs
CFLAGS += -DFAN1_PWM=DCR2
CFLAGS += -DFAN1_TABLE="\
	FAN_LEVEL(60, 0, 0), \
	FAN_LEVEL(60, 55, 40), \
	FAN_LEVEL(65, 60, 50), \
	FAN_LEVEL(70, 65, 60), \
	FAN_LEVEL(75, 70, 65), \
	FAN_LEVEL(80, 75, 65), \
"

# Add system76 common code
include src/board/system76/common/common.mk
