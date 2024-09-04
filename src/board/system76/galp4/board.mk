# SPDX-License-Identifier: GPL-3.0-only

board-y += ../galp3-c/board.c
board-y += ../galp3-c/gpio.c

# FIXME: Handle this better
CFLAGS += -I$(BOARD_DIR)/../galp3-c/include

EC=ite
CONFIG_EC_ITE_IT8587E=y
CONFIG_EC_FLASH_SIZE_128K = y

# Include keyboard
KEYBOARD=14in_86

# Set keyboard LED mechanism
CONFIG_HAVE_KBLED = y
KBLED=white_dac
CFLAGS+=-DKBLED_DAC=5

# Set battery I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_0

# Set touchpad PS2 bus
CFLAGS+=-DPS2_TOUCHPAD=PS2_3

# Set smart charger parameters
# FIXME: Verify parts and values.
CFLAGS+=\
	-DCHARGER_ADAPTER_RSENSE=10 \
	-DCHARGER_BATTERY_RSENSE=10 \
	-DCHARGER_CHARGE_CURRENT=1536 \
	-DCHARGER_CHARGE_VOLTAGE=13056 \
	-DCHARGER_INPUT_CURRENT=2100

# Set CPU power limits in watts
CFLAGS+=\
	-DPOWER_LIMIT_AC=40 \
	-DPOWER_LIMIT_DC=28

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
