# SPDX-License-Identifier: GPL-3.0-only

board-y += board.c
board-y += gpio.c

EC=ite
CONFIG_EC_ITE_IT5570E=y

# Enable eSPI
CONFIG_BUS_ESPI=y

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

# Set smart charger parameters
# TODO: actually bq24800
# FIXME: Verify parts and values.
CFLAGS+=\
	-DCHARGER_ADAPTER_RSENSE=5 \
	-DCHARGER_BATTERY_RSENSE=10 \
	-DCHARGER_CHARGE_CURRENT=2048 \
	-DCHARGER_CHARGE_VOLTAGE=13050 \
	-DCHARGER_INPUT_CURRENT=11500

# Set CPU power limits in watts
CFLAGS+=\
	-DPOWER_LIMIT_AC=180 \
	-DPOWER_LIMIT_DC=28

# Disable syncing fan speeds
#CFLAGS+=-DSYNC_FANS=0

# Smooth fans equal ramp-up/-down periods ~15s
CFLAGS+=-DSMOOTH_FANS_UP=60
CFLAGS+=-DSMOOTH_FANS_DOWN=60

# Custom fan curve
CFLAGS+=-DBOARD_HEATUP=7
CFLAGS+=-DBOARD_COOLDOWN=15
CFLAGS+=-DBOARD_FAN_POINTS="\
	FAN_POINT(45, 10), \
	FAN_POINT(50, 20), \
	FAN_POINT(55, 30), \
	FAN_POINT(60, 35), \
	FAN_POINT(65, 40), \
	FAN_POINT(70, 50), \
	FAN_POINT(74, 60), \
	FAN_POINT(80, 65), \
	FAN_POINT(87, 70), \
	FAN_POINT(90, 80), \
	FAN_POINT(94, 100), \
"

# Enable DGPU support
CFLAGS+=-DHAVE_DGPU=1
CFLAGS+=-DBOARD_DGPU_HEATUP=5
CFLAGS+=-DBOARD_DGPU_COOLDOWN=20
CFLAGS+=-DBOARD_DGPU_FAN_POINTS="\
	FAN_POINT(45, 10), \
	FAN_POINT(50, 20), \
	FAN_POINT(55, 30), \
	FAN_POINT(60, 35), \
	FAN_POINT(65, 40), \
	FAN_POINT(70, 50), \
	FAN_POINT(74, 60), \
	FAN_POINT(80, 65), \
	FAN_POINT(87, 70), \
	FAN_POINT(90, 80), \
	FAN_POINT(94, 100), \
"

# Add system76 common code
include src/board/system76/common/common.mk
