# Development environment

## OS

Linux is the only supported environment. Development is possible on a variety
of distros. It is recommended to use the latest Pop!\_OS or Ubuntu release.

The build server uses Ubuntu 20.04.

## IDE/Editor

Any editor that has support for [EditorConfig] may be used.

No specific IDE or editor is required or recommended.

## Toolset

EC development depends on using distro-provided packages for the most of the
toolset.

A complete list of dependencies can be seen in `scripts/install-deps.sh`.

### Cargo

rustup manages the [Rust] toolchain, which includes cargo. Cargo is required
for building the tools. It is the only part of the development toolset that is
not installed through distro packages.

### GNU Make

[GNU Make] is used to automate the build process. It also provides targets for
using ecflash (for flashing) and ectool (for console).

### SDCC

[SDCC] is the only practical open source toolset on Linux for targeting the
8051 architecture (among others). It contains a collection of tools comparable
to what you would expect from GCC for other development targets.

SDCC contains language extensions to support the 8051 architecture. Refer to
the [SDCC manual] for more information.

Version 4.0.0 or newer should be used.

**Note**: Fedora installs SDCC binaries to a non-standard location. Ensure that
`PATH` includes `/usr/libexec/sdcc`.


[EditorConfig]: https://editorconfig.org/
[GNU Make]: https://www.gnu.org/software/make/
[Rust]: https://www.rust-lang.org/
[SDCC manual]: http://sdcc.sourceforge.net/doc/sdccman.pdf
[SDCC]: http://sdcc.sourceforge.net/
