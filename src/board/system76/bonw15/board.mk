# SPDX-License-Identifier: GPL-3.0-only

board-y += board.c
board-y += gpio.c

EC=ite
CONFIG_EC_ITE_IT5570E=y
CONFIG_EC_FLASH_SIZE_128K = y

# Enable eSPI
CONFIG_BUS_ESPI=y

# Enable firmware security
CONFIG_SECURITY=y

# Include keyboard
KEYBOARD=15in_102

# Set keyboard LED mechanism
CONFIG_HAVE_KBLED = y
KBLED=bonw14

# Set battery I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_4

# Set touchpad PS2 bus
CFLAGS+=-DPS2_TOUCHPAD=PS2_3


# Set smart charger parameters
# XXX: PRS1 and PRS2 are in parallel for adapter Rsense?
CHARGER=oz26786
CFLAGS+=\
	-DCHARGER_ADAPTER_RSENSE=5 \
	-DCHARGER_BATTERY_RSENSE=5 \
	-DCHARGER_CHARGE_CURRENT=3072 \
	-DCHARGER_CHARGE_VOLTAGE=17400 \
	-DCHARGER_INPUT_CURRENT=16920

# Set USB-PD parameters
CONFIG_HAVE_USBPD = y
CONFIG_USBPD_TPS65987 = y
CFLAGS += -DI2C_USBPD=I2C_1

# Set CPU power limits in watts
CFLAGS+=\
	-DPOWER_LIMIT_AC=330 \
	-DPOWER_LIMIT_DC=55

# Custom fan curve
CFLAGS += -DBOARD_FAN1_HEATUP=5
CFLAGS += -DBOARD_FAN1_COOLDOWN=20
CFLAGS += -DBOARD_FAN2_POINTS="\
	FAN_POINT(60, 40), \
	FAN_POINT(65, 60), \
	FAN_POINT(70, 75), \
	FAN_POINT(75, 90), \
	FAN_POINT(80, 100), \
"

# Enable dGPU support
CONFIG_HAVE_DGPU = y
CFLAGS += -DI2C_DGPU=I2C_1

CFLAGS += -DBOARD_FAN2_HEATUP=5
CFLAGS += -DBOARD_FAN2_COOLDOWN=20
CFLAGS += -DBOARD_FAN2_POINTS="\
	FAN_POINT(60, 40), \
	FAN_POINT(65, 60), \
	FAN_POINT(70, 75), \
	FAN_POINT(75, 90), \
	FAN_POINT(80, 100), \
"

# Add system76 common code
include src/board/system76/common/common.mk
