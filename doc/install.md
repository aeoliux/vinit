# Installing vinit

## Dependencies
vinit only requires working C library and C compiler.

## Installation
meson's default prefix is '/usr/local', but prefix to installing an init system is preferred to be '/usr'.
```
meson build --buildtype=release --strip -Dprefix=/usr
ninja -C build install
```
Check before installing vinit if you don't have existing init system installed, because vinit installation will overwrite it!

## Configuration
See [inittab](inittab.md).