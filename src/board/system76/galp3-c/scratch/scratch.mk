SCRATCH_OBJ=$(patsubst src/%.c,$(BUILD)/%.rel,$(SCRATCH_SRC))
SCRATCH_CC=\
	sdcc \
	-mmcs51 \
	--model-small \
	--code-size 2048 \
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
$(SCRATCH_OBJ): $(BUILD)/%.rel: src/%.c $(SCRATCH_INCLUDE)
	@mkdir -p $(@D)
	$(SCRATCH_CC) $(SCRATCH_CFLAGS) -o $@ -c $<
