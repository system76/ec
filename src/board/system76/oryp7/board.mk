# SPDX-License-Identifier: GPL-3.0-only

EC=it5570e

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
CFLAGS+=\
	-DCHARGER_CHARGE_CURRENT=1536 \
	-DCHARGER_CHARGE_VOLTAGE=13050 \
	-DCHARGER_INPUT_CURRENT=9230

# Set CPU power limits in watts
CFLAGS+=\
	-DPOWER_LIMIT_AC=180 \
	-DPOWER_LIMIT_DC=28

# Number of seconds to average temperatures when deciding on fan duty.
# SMOOTH_FANS_* settings should generally be 4x the HEATUP/COOLDOWN settings.
CFLAGS+=\
    -DBOARD_HEATUP=5 \
    -DBOARD_DGPU_HEATUP=5 \
	-DSMOOTH_FANS_UP=20
CFLAGS+=\
    -DBOARD_COOLDOWN=10 \
    -DBOARD_DGPU_COOLDOWN=10 \
	-DSMOOTH_FANS_DOWN=40

# Don't smooth fan speed changes below 25% to mitigate buzzing
CFLAGS+=-DSMOOTH_FANS_MIN=25

# Custom fan curve
CFLAGS+=-DBOARD_FAN_POINTS="\
    FAN_POINT(50, 25), \
    FAN_POINT(51, 29), \
    FAN_POINT(52, 33), \
    FAN_POINT(53, 36), \
    FAN_POINT(54, 40), \
    FAN_POINT(55, 44), \
    FAN_POINT(56, 48), \
    FAN_POINT(57, 51), \
    FAN_POINT(58, 55), \
    FAN_POINT(59, 59), \
    FAN_POINT(60, 63), \
    FAN_POINT(61, 66), \
    FAN_POINT(62, 70), \
    FAN_POINT(63, 74), \
    FAN_POINT(64, 78), \
    FAN_POINT(65, 81), \
    FAN_POINT(66, 85), \
    FAN_POINT(67, 89), \
    FAN_POINT(68, 92), \
    FAN_POINT(69, 96), \
    FAN_POINT(70, 100) \
"

# Enable DGPU support
CFLAGS+=-DHAVE_DGPU=1
CFLAGS+=-DBOARD_DGPU_FAN_POINTS="\
    FAN_POINT(50, 25), \
    FAN_POINT(51, 29), \
    FAN_POINT(52, 33), \
    FAN_POINT(53, 36), \
    FAN_POINT(54, 40), \
    FAN_POINT(55, 44), \
    FAN_POINT(56, 48), \
    FAN_POINT(57, 51), \
    FAN_POINT(58, 55), \
    FAN_POINT(59, 59), \
    FAN_POINT(60, 63), \
    FAN_POINT(61, 66), \
    FAN_POINT(62, 70), \
    FAN_POINT(63, 74), \
    FAN_POINT(64, 78), \
    FAN_POINT(65, 81), \
    FAN_POINT(66, 85), \
    FAN_POINT(67, 89), \
    FAN_POINT(68, 92), \
    FAN_POINT(69, 96), \
    FAN_POINT(70, 100) \
"

# Add system76 common code
include src/board/system76/common/common.mk
