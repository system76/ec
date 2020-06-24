EC=it5570e

# Add keymap to src
KEYMAP?=default
SRC+=$(BOARD_DIR)/keymap/$(KEYMAP).c

# Set log level
# 0 - NONE
# 1 - ERROR
# 2 - WARN
# 3 - INFO
# 4 - DEBUG
# 5 - TRACE
CFLAGS+=-DLEVEL=4

# Enable I2C debug on 0x76
#CFLAGS+=-DI2C_DEBUGGER=0x76

# Set discrete GPU I2C bus
CFLAGS+=-DI2C_DGPU=I2C_1

# Set battery I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_4

# Set touchpad PS2 bus
CFLAGS+=-DPS2_TOUCHPAD=PS2_3

# Custom fan curve
CLFAGS+=-DBOARD_HEATUP=5
CFLAGS+=-DBOARD_COOLDOWN=20
CFLAGS+=-DBOARD_FAN_POINTS="\
	FAN_POINT(65, 40), \
	FAN_POINT(70, 60), \
	FAN_POINT(75, 75), \
	FAN_POINT(80, 90), \
	FAN_POINT(85, 100) \
"

# Set smart charger parameters
CFLAGS+=\
	-DCHARGER_CHARGE_CURRENT=1536 \
	-DCHARGER_CHARGE_VOLTAGE=12600 \
	-DCHARGER_INPUT_CURRENT=11800

# Enable DGPU support
CFLAGS+=-DHAVE_DGPU=1

# Set CPU power limits in watts
CFLAGS+=\
	-DPOWER_LIMIT_AC=180 \
	-DPOWER_LIMIT_DC=28

# Enable debug logging over keyboard parallel port
#CFLAGS+=-DPARPORT_DEBUG

# Add system76 common code
include src/board/system76/common/common.mk
