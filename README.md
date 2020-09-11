# System76 EC

System76 EC is a GPLv3 licensed embedded controller firmware for System76
laptops.

## Documentation

- [Supported embedded controllers](./doc/controllers.md)
- [Flashing firmware](./doc/flashing.md)
- [Debugging](./doc/debugging.md)
- [Creating a custom keyboard layout](./doc/keyboard-layout-customization.md)
- [Adding a new board](./doc/adding-a-new-board.md)

## Dependencies

The complete set of dependencies can be installed using the `deps.sh` script
from the [Open Firmware](https://github.com/system76/firmware-open) repo.

Dependencies specific to EC development can be installed with:

```
sudo apt install \
  avr-libc \
  avrdude \
  gcc-avr \
  sdcc
```
