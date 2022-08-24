# inittab - init system configuration file

## Example inittab
```
sysinit:/etc/rc sysinit
shutdown:/etc/rc shutdown
reboot:/etc/rc reboot
halt:/etc/rc halt

postn:9
post:wait:echo Welcome init!

post:respawn:agetty --noclear 38400 tty1 linux
post:respawn:agetty 38400 tty2 linux
post:respawn:agetty 38400 tty3 linux
post:respawn:agetty 38400 tty4 linux
post:respawn:agetty 38400 tty5 linux
post:respawn:agetty 38400 tty6 linux
post:oneshot:lightdm
```

## Explaining above example
### system initialization, shutdown, reboot and system halting
First line of given example is an information for init, what it have to execute to system be initialized. In this example '/etc/rc sysinit' shell command is used.<br />
Second defines shutdown command using exactly the same method, but just changed keyword 'sysinit' to 'shutdown'.<br />
Third and 4th line works also using the same method, but it is for rebooting and system halting.

### post scripts definition
Next lines are definitions of post-initialization scripts. Basic syntax for it is:
```
post:<method>:<command>
```
For methods we have:
- **wait** - is used when init must wait before command finishes,
- **respawn** - is used when init have to always reexecute command if it fails,
- **oneshot** - is used when command have to be just executed and init doesn't have to wait for and watch it.
<br />
Also one of the most important thing is that, before defining post scripts, you must tell to init how many post scripts you want to execute. Definition of this is:
```
postn:<number of post scripts>
```
