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
KBLED=rgb_pwm

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
	-DPOWER_LIMIT_DC=45

# Enable dGPU support
CONFIG_HAVE_DGPU = y
CFLAGS += -DI2C_DGPU=I2C_1

# Fan configs
CFLAGS += -DFAN1_PWM=DCR2
CFLAGS += -DBOARD_FAN1_HEATUP=5
CFLAGS += -DBOARD_FAN1_COOLDOWN=20
CFLAGS += -DBOARD_FAN1_POINTS="\
	FAN_POINT(50, 40), \
	FAN_POINT(55, 40), \
	FAN_POINT(60, 55), \
	FAN_POINT(69, 55), \
	FAN_POINT(74, 68), \
	FAN_POINT(79, 68), \
	FAN_POINT(80, 72), \
	FAN_POINT(87, 100), \
"

CFLAGS += -DFAN2_PWM=DCR4
CFLAGS += -DBOARD_FAN2_HEATUP=5
CFLAGS += -DBOARD_FAN2_COOLDOWN=20
CFLAGS += -DBOARD_FAN2_POINTS="\
	FAN_POINT(50, 40), \
	FAN_POINT(56, 40), \
	FAN_POINT(72, 100), \
"

# Add system76 common code
include src/board/system76/common/common.mk
