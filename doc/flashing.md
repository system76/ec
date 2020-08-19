# Flashing firmware

## Internal programmer

Use this method for flashing a system already running System76 EC.

This will trigger a watchdog reset causing the system to immediately power off.
Save and close all applications before flashing.

```
make BOARD=<vendor>/<model> flash_internal
```

## External programmer

Use this method for first-time flashing or flashing a bricked controller.

[A Mega 2560 is required for flashing.](./mega2560.md)

**The system must not have any power!**

1. Turn off the computer
2. Unplug the AC adapter
3. Remove the bottom panel
4. Disconnect the battery
5. Ground the laptop to the programmer
6. Disconnect the keyboard from its port
7. Attach the programmer to the keyboard port
8. Flash the firmware

```
make BOARD=<vendor>/<model> flash_external
```
