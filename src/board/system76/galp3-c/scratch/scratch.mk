SCRATCH_SRC+=\
	$(COMMON_DIR)/version.c \
	$(BOARD_DIR)/smfi.c

SCRATCH_BUILD=$(BUILD)/scratch
SCRATCH_OBJ=$(patsubst src/%.c,$(SCRATCH_BUILD)/%.rel,$(SCRATCH_SRC))
SCRATCH_CC=\
	sdcc \
	-mmcs51 \
	--model-small \
	--code-loc $(SCRATCH_OFFSET) \
	--code-size $(SCRATCH_SIZE) \
	--Werror

# Convert from binary file to C header
$(BUILD)/include/scratch.h: $(SCRATCH_BUILD)/scratch.rom
	@mkdir -p $(@D)
	xxd -s $(SCRATCH_OFFSET) --include < $< > $@

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
