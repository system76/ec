# Embedded Controllers

System76 EC is designed to be portable to boards using several 8-bit
microcontrollers, such as:

Board              | Embedded Controller | Architecture
------------------ | ------------------- | --------------
System76 ecsim     | Simulated ITE EC    | 8051
System76 Laptops   | ITE IT8587E         | 8051
System76 Laptops   | ITE IT5570E         | 8051

## Features

Shared features (that can be used across different micro-controllers):

Acronym | Feature                      | Description
------- | ---------------------------- | -----------
ADC     | Analog-digital converter     | Reads voltages from batteries and temperature from thermistors
DAC     | Digital-analog converter     | Brightness control of keyboard backlight
GPIO    | General purpose input/output | Control of LED's, buttons, and enable lines for other hardware
KBSC    | Keyboard scan controller     | Detects laptop keyboard presses. Keyboard scanning can be done using GPIOs, the ITE EC has hardware acceleration
LPC     | Low pin count                | Forwards memory and I/O access to EC. Most EC functions are exposed to the host through this interface. High frequency makes a GPIO implementation not possible. Simulation is provided by System76 ecsim
PECI    | Platform environment control interface | Reads CPU temperature (relative to throttle point). Proprietary nature means a GPIO implementation is unlikely, though not technically impossible
PS/2    | Keyboard and mouse bus       | Interfaces with touchpad. PS/2 can be done using GPIOs, the ITE EC has hardware acceleration
PWM     | Pulse-width modulation       | Fan control and brightness control of LED's
SMBUS   | System management bus        | Reads battery information
SPI     | Serial peripheral interface  | Reads and programs the EC's firmware
UART    | Universal asynchronous receiver-transmitter | Provides EC console input/output
