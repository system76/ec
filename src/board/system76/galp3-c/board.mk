EC=it8587e

# Add keymaps to include
INCLUDE+=$(wildcard $(BOARD_DIR)/keymap/*.h)

# Set debug level to debug
CFLAGS+=-DLEVEL=4

# Enable I2C debug on 0x76
# CFLAGS+=-DI2C_DEBUGGER=0x76
