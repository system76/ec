# Debugging the EC firmware

Terms used:
- *target*: The laptop system that has the EC to be tested
- *host*: The system that will have all devices connected to it and
    will receive the EC logs

## Parallel port

This method replaces the keyboard with a device used for debug logging.
An alternate method of interacting with the target is needed; e.g., an
external USB keyboard or SSH session.

Requirements:
- Arduino Mega 2560 compatible board
- 24 pin flexible printed circuit
    - 0.5mm or 1.0mm pitch, depending on target connector
- 24 pin FPC breakout board with connectors
- USB-C cable

### Configuring the Mega 2560

If the breakout board did not come preassembled, assemble it before
starting. This will require soldering.

1. Connect the breakout board to the Mega 2560
    - Orientation will affect the mapping of GPIO pins
2. Connect the Mega 2560 to the host
3. Configure GPIO pin mapping in `parallel.c` based on how it will
    connect to the target parallel port
    - Trace pin 1 on motherboard connector to Mega's GPIO pins
4. Build and flash the firmware for the Mega 2560
```
make BOARD=arduino/mega2560
make BOARD=arduino/mega2560 flash
```

### Setup

1. Enable parallel port debugging in the EC firmware
    - Uncomment `PARALLEL_DEBUG` in `src/board/system76/common/common.mk`
    - Build and flash the firmware for the target
2. Power off target
3. Remove bottom panel
4. Unplug keyboard cable
    - May require removing keyboard depending on port location
5. Connect Mega 2560 to host
    - This will create an ACM device at `/dev/ttyACM*`
6. Ground target to host
    - Connect USB cable from USB-C port on target to host
7. Connect Mega 2560 to target
8. Start the console
```
make BOARD=system76/<model> console_external
```

EC logs should now print to the console on the host. This can be tested
by removing or inserting the AC adapter to trigger a power event.

To return the Mega to host mode, reset the device.

If logs are corrupted, try power cycling the Mega or reseating the cable.

## I2C connection

**Failure to follow steps in order, or performing steps on an
unsupported board, may result in damaged board components.**

### Wiring the target

The I2C connection is made through the battery pins. Find the pins marked
`SMC_BAT` (clock) and `SMD_BAT` (data) in the service manual.

Board       | `SMC_BAT` | `SMD_BAT`
------------|-----------|-----------
addw1       | 4         | 5
addw2       | 4         | 5
darp5       | 4         | 5
darp6       | 4         | 5
galp3       | 4         | 5
galp3-b     | 4         | 5
galp3-c     | 4         | 5
galp4       | 4         | 5
gaze14      | 4         | 3
gaze15      | 4         | 3
lemp9       | 6         | 5
oryp5       | 4         | 5
oryp6       | 6         | 5

1. Power off system
2. Unplug AC adapter
3. Remove bottom panel
4. Disconnect battery
5. Attach one wire to `SMC_BAT`
6. Attach one wire to `SMD_BAT`
7. Route wires through the Kensington lock slot
    - Secure wires to board with tape
8. Attach female connector and housing to wires
9. Reconnect battery
10. Replace bottom panel

### Setup

Requirements:
- Target wired for EC debugging
- Adafruit Trinket M0
- USB-C cable

1. Enable I2C debugging in the EC firmware for the target
    - Uncomment `I2C_DEBUGGER` in `src/board/system76/common/common.mk`
    - Build and flash firmware
2. Connect Trinket M0 to host
    - This will create an ACM device at `/dev/ttyACM*`
3. Connect to ACM device from host
```
sudo tio -b 115200 -m INLCRNL /dev/ttyACM0
```
4. Ground target to host
    - Connect USB cable from USB-C port on target to host
5. Connect target to host
    - Connect `SMC_BAT` wire to `SCL` on Trinket M0
    - Connect `SMD_BAT` wire to `SDA` on Trinket M0

EC logs should now print to the console on the host. This can be tested
by removing or inserting the AC adapter to trigger a power event.
