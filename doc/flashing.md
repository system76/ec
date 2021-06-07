# Flashing firmware

## Internal programmer

Use this method for flashing a system already running System76 EC.

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
