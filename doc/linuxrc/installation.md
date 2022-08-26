# vinit linuxrc installation

## Dependencies
- shell script interpreter
- POSIX coreutils
- ifconfig, route (for static network connection)
- dhcpcd (for dynamic network connection)
- vinit (vinit linuxrc can be used with SysVinit, but it's recommended to use it with vinit compiled with `/` prefix)
- agetty (POSIX getty can be used, if `/sbin/agetty` is symlink to `/sbin/getty` or by fixing `/etc/inittab`)

## Installation
1. Enter to vinit source code root directory,
2. Run commands:
```sh
install -m755 linuxrc/rc.sh linuxrc/rc.halt.sh /etc
install -m644 linuxrc/inittab /etc
```

## Post-installation configurations
See:
- [linuxrc configuration](configuration.md)
- [Network setup](networking.md)

And boot device using vinit.