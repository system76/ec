# Firmware security

The firmware security feature can be configured by setting `CONFIG_SECURITY=1`
in the `src/board/system76/<board>/Makefile.mk` file. This feature prevents
programming the EC firmware at runtime, unless the EC is unlocked with the
`system76-ectool security unlock` command. After this, on the next reboot, the
EC will respond to the SPI and reset commands. On boards where the `ME_WE` GPIO
exists, it will be set high when the EC security state is unlocked.

Other firmware components can use this state to perform their own locking and
unlocking primitives. For example, in `coreboot`, flash regions may be locked
when the EC security state is locked. In `EDK2`, a physical presence dialog may
be shown when the EC security state is unlocked.
