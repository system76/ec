# Keyboard layout customization

System76 EC firmware supports using custom keyboard layouts. For example,
having the physical "Caps Lock" key function as "Escape" when pressed.

This only applies to the internal keyboard. External keyboards are not
affected.

Advanced functionality, such as macros or QMK's Mod-Tap, are not supported.

You must be able to build and flash firmware. See the [README](../README.md)
if you have not built the firmware before.

## Firmware version

To ensure compatibility with other system components (e.g. your current BIOS
version), it's recommended to flash the same firmware version as you're
currently using.

Determine the BIOS version using dmidecode. The BIOS version is formatted as
`<date>_<revision>`.

```sh
sudo dmidecode -t bios | grep Version
```

From the [firmware-open](https://github.com/system76/firmware-open) repo,
determine the EC commit used for the BIOS version.

```sh
git ls-tree <bios_rev> ec
```

Checkout that commit in the EC repo and update the submodules.

```sh
git checkout <ec_rev>
git submodule update --recursive
```

## Adding a layout file

Determine the keyboard used for your model.

```sh
grep KEYBOARD src/board/system76/<model>/board.mk
```

Copy the default layout file for the keyboard to a separate file in the keymap
directory.

```sh
cp src/keyboard/system76/<keyboard>/keymap/default.c src/keyboard/system76/<keyboard>/keymap/<custom>.c
```

The name of the keymap can be anything. Following QMK convention, the file name
could be your username.

## Modifying the layout

A keymap file is a C file that defines the position of key and its function.
Only the `LAYOUT`s should be modified.

There are 2 layers in a keymap file.

- `KEYMAP[0]`: The first `LAYOUT` defines the default layer
- `KEYMAP[1]`: The second `LAYOUT` defines the function layer

A `KT_FN` key must be assigned on the first layer in order to access the second
layer. The `KT_FN` key must be held to use keys on the second layer.

Change one key at a time to avoid losing your place.

Some related files are:

- `src/common/include/common/keymap.h`: Defines the key scancodes
- `src/keyboard/system76/<keyboard>/include/board/keymap.h`: Defines the
    keyboard matrix

## Building with the new layout

Build the firmware specifying the `KEYMAP` you have added.

```sh
make BOARD=system76/lemp9 KEYMAP=custom
```

Use a config file to simplify this. In the EC project's root directory, create
a file named `config.mk`. Define the `BOARD` and `KEYMAP` variables.

```mk
BOARD?=system76/lemp9
KEYMAP?=custom
```

Then building the firmware only requires calling make.

```sh
# Make will read config.mk to determine which board and keymap to use.
make
```

## Flashing the new firmware

See [flashing firmware](./flashing.md) for details.

Do not use the keyboard or touchpad while it is flashing.

The system will power off as part of the flash process. Turn it back on after
it has powered off.

The keyboard can now be used with your new layout.

## Troubleshooting

If your layout does not work as you intended, only the internal keyboard will
be affected. An external USB keyboard or SSH session can be used to correct the
layout and reflash the firmware.
