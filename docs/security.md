# Firmware security

The firmware security feature can be configured by setting `CONFIG_SECURITY=1`
in the `src/board/system76/<board>/Makefile.mk` file. This feature prevents
programming the EC firmware at runtime, unless the EC is unlocked with the
`system76-ectool security unlock` command. After this, on the next reboot, the
EC will respond to the SPI and reset commands.

This feature will drive the `ME_WE` pin high when the state is unlocked. On
Intel hosts, this pin is connected to `HDA_SDO` and will disable security
policies set in the flash descriptor.

- `HDA_SDO`: Flash Descriptor Security Override

Other firmware components can use this state to perform their own locking and
unlocking primitives. For example, in coreboot, flash regions may be locked
when the EC security state is locked. In the UEFI payload, a physical presence
dialog may be shown when the EC security state is unlocked.
