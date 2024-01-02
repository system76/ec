# Flashing firmware

## Internal programmer

Use this method for flashing a system already running System76 EC.

This method will only work if the running firmware is not locked. Firmware is
write locked if it was built with `CONFIG_SECURITY=y`. firmware-update must be
used to flash from UEFI in this state (see `flash.sh` in firmware-open).

This will trigger a watchdog reset causing the system to **immediately power
off**. OS data may be lost or corrupted as a result. Save and close all
applications before flashing.

```
make BOARD=<vendor>/<model> flash_internal
```

## External programmer

Use this method for:

- flashing from proprietary firmware to System76 EC firmware
- flashing without risking loss of OS data
- flashing a bricked controller

This requires:

- [A configured Mega 2560](./mega2560.md): For programming the EC itself
- A USB cable: For creating a common ground and providing power
    - USB-C is recommended, but USB-A will work as well
- A second computer: For building and flashing the firmware

**The system must not have any power!**

1. Turn off the laptop
2. Unplug the AC adapter
3. Remove the bottom panel
4. Disconnect the battery
5. Disconnect the keyboard from its port
6. Replace the bottom panel and flip the laptop back over
7. Connect the USB cable from the laptop to the host machine
8. Connect the Mega 2560 to the host machine
9. Attach the programmer to the keyboard port
10. Flash the firmware

```
make BOARD=<vendor>/<model> flash_external
```

One of the first things it does is read the EC ID and version. If working, the
ID will be the EC model the machine uses.

```
ID: 5570 VER: 2
ID: 8587 VER: 6
```

Any other values means that the Mega 2560 is misconfigured or the FPC is not
seated correctly. E.g., this is wrong:

```
ID: FF7F VER: 127
```
