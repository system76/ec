# SPDX-License-Identifier: GPL-3.0-only

board-y += ../darp5/board.c
board-y += ../darp5/gpio.c

# FIXME: Handle this better
CFLAGS += -I$(BOARD_DIR)/../darp5/include

EC=ite
CONFIG_EC_ITE_IT8587E=y
CONFIG_EC_FLASH_SIZE_128K = y

# Include keyboard
KEYBOARD=15in_102

# Set keyboard LED mechanism
CONFIG_HAVE_KBLED = y
KBLED=darp5
CFLAGS+=-DI2C_KBLED=I2C_1

# Set battery I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_0

# Set touchpad PS2 bus
CFLAGS+=-DPS2_TOUCHPAD=PS2_3

# Set smart charger parameters
CFLAGS+=\
	-DCHARGER_ADAPTER_RSENSE=10 \
	-DCHARGER_BATTERY_RSENSE=10 \
	-DCHARGER_CHARGE_CURRENT=3072 \
	-DCHARGER_CHARGE_VOLTAGE=17600 \
	-DCHARGER_INPUT_CURRENT=3420

# Set CPU power limits in watts
CFLAGS+=\
	-DPOWER_LIMIT_AC=65 \
	-DPOWER_LIMIT_DC=45

CFLAGS += -DFAN1_PWM=DCR2

# Add system76 common code
include src/board/system76/common/common.mk
