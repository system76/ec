# Debugging the EC firmware

**Failure to follow steps in order, or performing steps on an
unsupported board, may result in damaged board components.**

Terms used:

- *target*: The laptop system that has the EC to be tested.
- *host*: The system that will have all devices connected to it and
    will receive the EC logs.

## Wiring the target

These steps apply to the following models:
- darp5
- darp6
- galp3-c
- galp4

1. Power off system
2. Unplug AC adapter
3. Remove bottom panel
4. Disconnect battery
5. Attach one wire to battery pin 4 (`CLK`)
6. Attach one wire to battery pin 5 (`DATA`)
7. Route wires through the Kensington lock slot
    - Secure wires to board with tape
8. Attach female connector and housing to wires
9. Reconnect battery
10. Replace bottom panel

## Setup

Requirements:
- Target wired for EC debugging
- Adafruit Trinket M0
- USB-C cable

1. Enable I2C debugging in the EC firmware for the target
    - Uncomment `I2C_DEBUGGER` in `board.mk`
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
    - Connect `CLK` wire to `SCL` on Trinket M0
    - Connect `DATA` wire to `SDA` on Trinket M0

EC logs should now print to the console on the host. This can be tested
by removing or inserting the AC adapter to trigger a power event.
