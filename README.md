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

Install dependencies using the provided script:

```
./scripts/deps.sh
```

If rustup was installed as part of this, then the correct `cargo` will not be
available in the running session. Start a new shell session or source the env
file to update `PATH`.

```
source $HOME/.cargo/env
```

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
