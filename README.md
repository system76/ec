# System76 EC (WIP)

System76 EC is a GPLv3 licensed embedded controller firmware for System76
laptops. It is designed to be portable to boards using several 8-bit
microcontrollers, such as:

Board              | Embedded Controller | Architecture
------------------ | ------------------- | --------------
System76 ecsim     | Simulated ITE EC    | 8051
System76 Laptops   | ITE IT8587E         | 8051
System76 Thelio Io | Atmel ATMEGA32U4    | AVR
Arduino Mega 2560  | Atmel ATMEGA2560    | AVR

Shared features (that can be used across different micro-controllers):

Acronym | Feature                      | Description
------- | ---------------------------- | -----------
ADC     | Analog-digital converter     | Reads voltages from batteries and temperature from thermistors
DAC     | Digital-analog converter     | Brightness control of keyboard backlight
GPIO    | General purpose input/output | Control of LED's, buttons, and enable lines for other hardware
KBSC    | Keyboard scan controller     | Detects laptop keyboard presses. Keyboard scanning can be done using GPIOs, the ITE EC has hardware acceleration
PS/2    | Keyboard and mouse bus       | Interfaces with touchpad. PS/2 can be done using GPIOs, the ITE EC has hardware acceleration
PWM     | Pulse-width modulation       | Fan control and brightness control of LED's
SMBUS   | System management bus        | Reads battery information
SPI     | Serial peripheral interface  | Reads and programs the EC's firmware
UART    | Universal asynchronous receiver-transmitter | Provides EC console input/output

Features specific to ITE embedded controllers:

Acronym | Feature                      | Description
------- | ---------------------------- | -----------
LPC     | Low pin count                | Forwards memory and I/O access to EC. Most EC functions are exposed to the host through this interface. High frequency makes a GPIO implementation not possible. Simulation is provided by System76 ecsim
PECI    | Platform environment control interface | Reads CPU temperature (relative to throttle point). Proprietary nature means a GPIO implementation is unlikely, though not technically impossible

Features specific to Atmel embedded controllers:

Acronym | Feature                      | Description
------- | ---------------------------- | -----------
USB     | Universal serial bus         | Can be used to provide access to EC functions either to the host or to a remote machine

## Flashing

Requirements:

- [sdcc](http://sdcc.sourceforge.net/)
- [Rust](https://www.rust-lang.org/)

### Internal programmer

Use this method for flashing a system already running System76 EC.

```
make BOARD=<vendor>/<model> flash_internal
```

### External programmer

Use this method for first-time flashing or flashing a bricked controller.

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
