# SPDX-License-Identifier: GPL-3.0-only

board-y += ../galp3-c/board.c
board-y += ../galp3-c/gpio.c

# FIXME: Handle this better
CFLAGS += -I$(BOARD_DIR)/../galp3-c/include

EC=ite
CONFIG_EC_ITE_IT8587E=y
CONFIG_EC_FLASH_SIZE_128K = y

# Intel-based host
CONFIG_PLATFORM_INTEL = y

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
CFLAGS += -DBOARD_FAN1_POINTS="\
	FAN_POINT(40, 15), \
	FAN_POINT(44, 16), \
	FAN_POINT(48, 17), \
	FAN_POINT(52, 18), \
	FAN_POINT(56, 21), \
	FAN_POINT(60, 26), \
	FAN_POINT(63, 31), \
	FAN_POINT(66, 36), \
	FAN_POINT(69, 41), \
	FAN_POINT(72, 48), \
	FAN_POINT(75, 56), \
	FAN_POINT(78, 64), \
	FAN_POINT(81, 72), \
	FAN_POINT(84, 81), \
	FAN_POINT(87, 90), \
	FAN_POINT(90, 100), \
"

# Add system76 common code
include src/board/system76/common/common.mk
