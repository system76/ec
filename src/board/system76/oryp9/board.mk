# SPDX-License-Identifier: GPL-3.0-only

board-y += board.c
board-y += gpio.c

EC=ite
CONFIG_EC_ITE_IT5570E=y

# Enable eSPI
CFLAGS+=-DEC_ESPI=1

# FIXME: Use S3 instead of S0ix
CFLAGS+=-DUSE_S0IX=1
# Apply PMC hack for S0ix
CFLAGS+=-DPMC_S0IX_HACK=1

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

# Custom fan curve
CFLAGS+=-DBOARD_HEATUP=5
CFLAGS+=-DBOARD_COOLDOWN=20
CFLAGS+=-DBOARD_FAN_POINTS="\
	FAN_POINT(50, 40), \
	FAN_POINT(55, 40), \
	FAN_POINT(60, 55), \
	FAN_POINT(69, 55), \
	FAN_POINT(74, 68), \
	FAN_POINT(79, 68), \
	FAN_POINT(80, 72), \
	FAN_POINT(87, 100), \
"

# Enable DGPU support
CFLAGS+=-DHAVE_DGPU=1
CFLAGS+=-DBOARD_DGPU_HEATUP=5
CFLAGS+=-DBOARD_DGPU_COOLDOWN=20
CFLAGS+=-DBOARD_DGPU_FAN_POINTS="\
	FAN_POINT(50, 40), \
	FAN_POINT(56, 40), \
	FAN_POINT(72, 100), \
"

# Add system76 common code
include src/board/system76/common/common.mk
