# How to customize your keyboard layout with System76 EC

## Dependencies
* Install [sdcc](http://sdcc.sourceforge.net/): `sudo apt install sdcc`
* Install rust nightly
  - Install [rustup](https://rustup.rs/)
  - Reboot
  - Run `rustup install nightly`)
  
## Adding your layout
* In `src/board/system76/{your-model}/keymap/`, copy `default.c` and rename it. I named mine `levi.c`.
* In `src/common/include/common/keymap.h` you will find the key definitions.
TODO: elaborate on this

## Point EC at your keymap
* In `ec/src/board/system76/{model}/board.mk` change `KEYMAP?=default` to the name of your layout.
TODO: There's probably a better way to do this

## Test build your EC
* From the `ec` directory, run `make BOARD=system76/{your-model}` to make sure it builds correctly.

## Flash your EC
* If the test build went well, you should be ready to flash your EC:
  - Close all running applications.
  - Unplug everything from your laptop except the charger.
  - Flash with `make BOARD=system76/{your-model} flash_internal`
  - When it says, "Waiting 5 seconds for all keys to be released", it is important to not touch the keyboard or trackpad until it turns itself off.
  - Once it shuts down, you can power it back on, and your keymap will be active.
