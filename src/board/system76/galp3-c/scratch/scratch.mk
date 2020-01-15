# Enable I2C debugging
SCRATCH_SRC+=\
	src/common/i2c.c \
	src/ec/$(EC)/i2c.c
SCRATCH_INCLUDE+=\
	src/common/include/common/*.h \
	src/ec/$(EC)/include/ec/*.h
SCRATCH_CFLAGS+=\
	-Isrc/common/include \
	-Isrc/ec/$(EC)/include \
	-DI2C_DEBUGGER=0x76

SCRATCH_BUILD=$(BUILD)/scratch
SCRATCH_OBJ=$(patsubst src/%.c,$(SCRATCH_BUILD)/%.rel,$(SCRATCH_SRC))
SCRATCH_CC=\
	sdcc \
	-mmcs51 \
	--model-small \
	--code-size 4096 \
	--Werror

# Convert from binary file to C header
$(BUILD)/include/scratch.h: $(SCRATCH_BUILD)/scratch.rom
	@mkdir -p $(@D)
	xxd --include < $< > $@

# Convert from Intel Hex file to binary file
$(SCRATCH_BUILD)/scratch.rom: $(SCRATCH_BUILD)/scratch.ihx
	@mkdir -p $(@D)
	makebin -p < $< > $@

# Link object files into Intel Hex file
$(SCRATCH_BUILD)/scratch.ihx: $(SCRATCH_OBJ)
	@mkdir -p $(@D)
	$(SCRATCH_CC) -o $@ $^

# Compile C files into object files
$(SCRATCH_OBJ): $(SCRATCH_BUILD)/%.rel: src/%.c $(SCRATCH_INCLUDE)
	@mkdir -p $(@D)
	$(SCRATCH_CC) $(SCRATCH_CFLAGS) -o $@ -c $<
