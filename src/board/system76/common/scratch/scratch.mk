# Set scratch ROM parameters
SCRATCH_OFFSET=1024
SCRATCH_SIZE=1024
CFLAGS+=-DSCRATCH_OFFSET=$(SCRATCH_OFFSET) -DSCRATCH_SIZE=$(SCRATCH_SIZE)

# Copy parameters to use when compiling scratch ROM
SCRATCH_INCLUDE=$(INCLUDE)
SCRATCH_CFLAGS=$(CFLAGS)

#Include scratch source
SCRATCH_DIR=$(SYSTEM76_COMMON_DIR)/scratch
SCRATCH_SRC=$(wildcard $(SCRATCH_DIR)/*.c)
SCRATCH_INCLUDE+=$(wildcard $(SCRATCH_DIR)/include/scratch/*.h) $(SCRATCH_DIR)/scratch.mk
SCRATCH_CFLAGS+=-I$(SCRATCH_DIR)/include -D__SCRATCH__

# Add minimal source from other directories
SCRATCH_SRC+=\
	$(COMMON_DIR)/version.c \
	$(SYSTEM76_COMMON_DIR)/smfi.c

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

# Include scratch header in main firmware
CFLAGS+=-I$(BUILD)/include
INCLUDE+=$(BUILD)/include/scratch.h
