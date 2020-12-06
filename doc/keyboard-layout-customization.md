# Keyboard layout customization

## Dependencies
* Dependencies are listed in the README file for the EC project.

## Select firmware version

To ensure compatibility with other system components (e.g. your current BIOS
version), it's recommended to flash the same firmware version as you're
currently using.
* Go to Settings -> Firmware and check the current version. It will look
  something like "2020-09-17_f10af76".
* Clone the [system76/firmware-open](https://github.com/system76/firmware-open)
  repository.
* Checkout the version that matches your firmware, e.g.: `git checkout f10af76`.
* Run `git submodule init && git submodule update && cd ec`.

## Adding your layout
* In `src/board/system76/{your-model}/keymap/`, copy `default.c` and rename it.
  There are two examples to reference in the `lemp9` directory: `jeremy.c` and 
  `levi.c`.
* In `src/common/include/common/keymap.h` you will find a list of the key
  definitions.
* You will notice two sets of keys in these layout files. The top one is the 
  standard mapping. The bottom one is the Fn layer, meaning it is active when
  the Fn key is being held. If you look at the Fn layer in the `levi.c` layout,
  you will see that there are arrow keys at WASD, media and volume keys on the
  bottom row, etc. 
* Hint: To avoid losing your place change one key at a time, referencing the key
  code you are deleting to keep yourself positioned correctly in the list of
  keycodes.

## Configure your EC to build with your layout
* Create a file in the project's root directory called `config.mk` and add your
  board and keyboard layout to it. For example, if you want to build lemp9
  firmware with Jeremy's layout (which is at 
  `ec/src/board/system76/lemp9/keymap/jeremy.c`):
```
BOARD?=system76/lemp9
KEYMAP?=jeremy
```

## Test build your EC
* From the `ec` directory, run `make` to make sure it builds correctly.

## Flash your EC
* If the test build went well, you should be ready to flash your EC:
  - Close all running applications.
  - Unplug everything from your laptop except the charger.
  - Flash with `make flash_internal`
  - When it says, "Waiting 5 seconds for all keys to be released", it is
    important to not touch the keyboard or trackpad until it finishes writing
    the new EC and turns itself off. 
  - Once it shuts down, you can power it back on, and your keymap will be
    active. 
  - If you changed your layout in such a way that you can't easily type, just
    plug in a USB keyboard and re-flash to the default layout until you can
    fix. The USB keyboard's layout will be unaffected.
