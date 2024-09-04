# SPDX-License-Identifier: GPL-3.0-only

board-y += board.c
board-y += gpio.c

EC=ite
CONFIG_EC_ITE_IT8587E=y
CONFIG_EC_FLASH_SIZE_128K = y

# Include keyboard
KEYBOARD=15in_102

# Set keyboard LED mechanism
CONFIG_HAVE_KBLED = y
KBLED=rgb_pwm

# Set battery I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_0

# Set touchpad PS2 bus
CFLAGS+=-DPS2_TOUCHPAD=PS2_3

# Set smart charger parameters
CFLAGS+=\
	-DCHARGER_ADAPTER_RSENSE=5 \
	-DCHARGER_BATTERY_RSENSE=10 \
	-DCHARGER_CHARGE_CURRENT=3072 \
	-DCHARGER_CHARGE_VOLTAGE=12600 \
	-DCHARGER_INPUT_CURRENT=11800

# Set CPU power limits in watts
CFLAGS+=\
	-DPOWER_LIMIT_AC=180 \
	-DPOWER_LIMIT_DC=45

# Enable dGPU support
CONFIG_HAVE_DGPU = y
CFLAGS += -DI2C_DGPU=I2C_1

# Fan configs
CFLAGS += -DFAN1_PWM=DCR2
CFLAGS += -DFAN1_TABLE="\
	FAN_LEVEL(60, 0, 0), \
	FAN_LEVEL(60, 55, 40), \
	FAN_LEVEL(65, 60, 60), \
	FAN_LEVEL(70, 65, 75), \
	FAN_LEVEL(75, 70, 90), \
	FAN_LEVEL(80, 75, 100), \
"

CFLAGS += -DFAN2_PWM=DCR4
CFLAGS += -DFAN2_TABLE="\
	FAN_LEVEL(60, 0, 0), \
	FAN_LEVEL(60, 55, 40), \
	FAN_LEVEL(65, 60, 60), \
	FAN_LEVEL(70, 65, 75), \
	FAN_LEVEL(75, 70, 90), \
	FAN_LEVEL(80, 75, 100), \
"

# Add system76 common code
include src/board/system76/common/common.mk
