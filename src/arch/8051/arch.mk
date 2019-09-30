CC=sdcc -mmcs51
OBJ=$(patsubst src/%.c,$(BUILD)/%.rel,$(SRC))

# Run EC rom in simulator
sim: $(BUILD)/ec.rom
	cargo run \
		--release \
		--manifest-path ecsim/Cargo.toml \
		--no-default-features \
		-- $<

# Convert from Intel Hex file to binary file
$(BUILD)/ec.rom: $(BUILD)/ec.ihx
	@mkdir -p $(@D)
	makebin -p < $< > $@

# Link object files into Intel Hex file
$(BUILD)/ec.ihx: $(OBJ)
	@mkdir -p $(@D)
	$(CC) -o $@ $^

# Compile C files into object files
$(BUILD)/%.rel: src/%.c $(INCLUDE)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<
