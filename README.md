# System76 EC

System76 EC is a GPLv3 licensed embedded controller firmware for System76
laptops.

## Documentation

- [Supported embedded controllers](./docs/controllers.md)
- [Flashing firmware](./docs/flashing.md)
- [Debugging](./docs/debugging.md)
- [Creating a custom keyboard layout](./docs/keyboard-layout-customization.md)
- [Development environment](./docs/dev-env.md)
- [Adding a new board](./docs/adding-a-new-board.md)

## Quickstart

Install GNU Make, then install the rest of the dependencies and set up the
environment for development using the Make target.

```sh
make set-up
```

If rustup was installed as part of this, then the correct `cargo` will not be
available in the running session. Start a new shell session or source the env
file to update `PATH`.

```sh
source $HOME/.cargo/env
```

Then build the firmware for your laptop model.

```sh
make BOARD=system76/<model>
```

See [Flashing](./docs/flashing.md) for how to use the new firmware image.

## Releases

The EC firmware itself does not have tagged releases. Any commit of this repo
may be used as a part of a [System76 Open Firmware][firmware-open] release.

In official releases the EC shares the same version as the BIOS firmware. Run
the follow command from firmware-open to determine the corresponding EC commit
for a release.

```
git ls-tree <release_hash> ec
```

[firmware-open]: https://github.com/system76/firmware-open

## Legal

System76 EC is copyright System76 and contributors.

System76 EC firmware is made available under the terms of the GNU General
Public License, version 3. See [LICENSE](./LICENSE) for details.

- firmware: GPL-3.0-only
- ecflash: LGPL-2.1-or-later
- ecsim: MIT
- ectool: MIT

Datasheets for the ITE embedded controllers used in System76 laptops cannot be
shared outside of company. (However, the IT81202E datasheet is [publicly
available][it81202e]. While it uses a different core, a significant portion of
the register information is the same as IT85878E/IT5570E.)

[it81202e]: https://www.ite.com.tw/en/product/view?mid=149
