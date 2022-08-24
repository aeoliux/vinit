# Using vinit with OpenRC instead of SysVinit or openrc init
OpenRC by default is configured to work with SysVinit or openrc-init.<br />
vinit provides almost all features from SysVinit, so we can use it with the OpenRC init system just by configuring [inittab](inittab.md).

## /etc/inittab
To use this init with OpenRC, just delete everything from '/etc/inittab' (if it exists) and paste following content:
```
sysinit:openrc sysinit
shutdown:openrc shutdown
reboot:openrc reboot
halt:openrc shutdown

postn:2
post:wait:openrc boot
post:wait:openrc default
```
Also if you previously had a '/etc/inittab' file not empty, replace `postn:2` with `postn:8` and add following content to the end of '/etc/inittab':
```
post:respawn:agetty --noclear 38400 tty1 linux
post:respawn:agetty 38400 tty2 linux
post:respawn:agetty 38400 tty3 linux
post:respawn:agetty 38400 tty4 linux
post:respawn:agetty 38400 tty5 linux
post:respawn:agetty 38400 tty6 linux
```

Now it's required to just install vinit.
See [installation](install.md)