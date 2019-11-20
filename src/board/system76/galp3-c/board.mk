EC=it8587e

# Add keymaps to include
INCLUDE+=$(wildcard $(BOARD_DIR)/keymap/*.h)

# Set debug level to warn
CFLAGS+=-DLEVEL=2

# Enable I2C debug on 0x76
# CFLAGS+=-DI2C_DEBUGGER=0x76

# Add scratch ROM source
SCRATCH_SRC=$(wildcard $(BOARD_DIR)/scratch/*.c)
SCRATCH_OBJ=$(patsubst src/%.c,$(BUILD)/%.rel,$(SCRATCH_SRC))
SCRATCH_CC=\
	sdcc \
	-mmcs51 \
	--model-small \
	--code-size 1024 \
	--Werror

# Convert from binary file to C header
$(BUILD)/include/scratch.h: $(BUILD)/scratch.rom
	@mkdir -p $(@D)
	xxd --include < $< > $@

# Convert from Intel Hex file to binary file
$(BUILD)/scratch.rom: $(BUILD)/scratch.ihx
	@mkdir -p $(@D)
	makebin -p < $< > $@

# Link object files into Intel Hex file
$(BUILD)/scratch.ihx: $(SCRATCH_OBJ)
	@mkdir -p $(@D)
	$(SCRATCH_CC) -o $@ $^

# Compile C files into object files
$(SCRATCH_OBJ): $(BUILD)/%.rel: src/%.c
	@mkdir -p $(@D)
	$(SCRATCH_CC) -o $@ -c $<

# Require scratch.rom to be compiled before main firmware
CFLAGS+=-I$(BUILD)/include
INCLUDE+=$(BUILD)/include/scratch.h
