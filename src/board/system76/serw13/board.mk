# SPDX-License-Identifier: GPL-3.0-only

board-y += board.c
board-y += gpio.c

EC=ite
CONFIG_EC_ITE_IT5570E=y

# Enable eSPI
CONFIG_BUS_ESPI=y

# Enable firmware security
CONFIG_SECURITY=y

# Include keyboard
KEYBOARD=15in_102

# Set keyboard LED mechanism
KBLED=rgb_pwm

# Set discrete GPU I2C bus
CFLAGS+=-DI2C_DGPU=I2C_1

# Set battery I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_4

# Set touchpad PS2 bus
CFLAGS+=-DPS2_TOUCHPAD=PS2_3

# Set USB-PD I2C bus
CFLAGS+=-DI2C_USBPD=I2C_1

# Set smart charger parameters
# TODO: actually bq24800
CFLAGS+=\
	-DCHARGER_ADAPTER_RSENSE=10 \
	-DCHARGER_BATTERY_RSENSE=10 \
	-DCHARGER_CHARGE_CURRENT=2048 \
	-DCHARGER_CHARGE_VOLTAGE=13050 \
	-DCHARGER_INPUT_CURRENT=14000

# Set USB-PD parameters
USBPD=tps65987

# Set CPU power limits in watts
CFLAGS+=\
	-DPOWER_LIMIT_AC=280 \
	-DPOWER_LIMIT_DC=55

CFLAGS+=-DSMOOTH_FANS_MIN=28

# Custom fan curve
CFLAGS+=-DBOARD_HEATUP=5
CFLAGS+=-DBOARD_COOLDOWN=20
CFLAGS+=-DBOARD_FAN_POINTS="\
	FAN_POINT(50, 28), \
	FAN_POINT(55, 28), \
	FAN_POINT(60, 40), \
	FAN_POINT(68, 40), \
	FAN_POINT(73, 50), \
	FAN_POINT(87, 100), \
"

# Enable DGPU support
CFLAGS+=-DHAVE_DGPU=1
CFLAGS+=-DBOARD_DGPU_HEATUP=5
CFLAGS+=-DBOARD_DGPU_COOLDOWN=20
CFLAGS+=-DBOARD_DGPU_FAN_POINTS="\
	FAN_POINT(50, 28), \
	FAN_POINT(55, 28), \
	FAN_POINT(60, 40), \
	FAN_POINT(68, 40), \
	FAN_POINT(73, 50), \
	FAN_POINT(87, 100), \
"

# Add system76 common code
include src/board/system76/common/common.mk
