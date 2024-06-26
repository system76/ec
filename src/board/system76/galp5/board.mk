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
# FIXME: Verify parts and values.
CHARGER=oz26786
CFLAGS+=\
	-DCHARGER_ADAPTER_RSENSE=10 \
	-DCHARGER_BATTERY_RSENSE=10 \
	-DCHARGER_CHARGE_CURRENT=1536 \
	-DCHARGER_CHARGE_VOLTAGE=17400 \
	-DCHARGER_INPUT_CURRENT=3420

# Set CPU power limits in watts
CFLAGS+=\
	-DPOWER_LIMIT_AC=65 \
	-DPOWER_LIMIT_DC=45

# Custom fan curve
CFLAGS+=-DBOARD_FAN_POINTS="\
	FAN_POINT(70, 25), \
	FAN_POINT(80, 25), \
	FAN_POINT(80, 40), \
	FAN_POINT(88, 40), \
	FAN_POINT(88, 100) \
"

# Enable dGPU support
CONFIG_HAVE_DGPU = y
CFLAGS += -DI2C_DGPU=I2C_1
CFLAGS+=-DBOARD_DGPU_FAN_POINTS="\
	FAN_POINT(70, 25), \
	FAN_POINT(75, 40), \
	FAN_POINT(80, 75), \
	FAN_POINT(85, 90), \
	FAN_POINT(90, 100) \
"

# Add system76 common code
include src/board/system76/common/common.mk
