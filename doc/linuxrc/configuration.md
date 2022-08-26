# vinit linuxrc configuration
vinit linuxrc stores it configuration at `/etc/rc.conf`.

## Hostname
Device hostname is configured via `HOSTNAME` variable in `/etc/rc.conf`. Example:
```conf
HOSTNAME="examplehostname"
```
Ensure if you have the `hostname` command, because it is required to set device hostname via vinit linuxrc.

## Mounting `/tmp`
If you want to mount tmpfs at `/tmp`, set `MOUNT_TMPFS` to any value (not empty).

## Hardware clock
`HWCLOCK` variable can be set to 2 values:
- `UTC` - select it if your hardware clock is set to UTC,
- `localtime` - select it if your hardware clock is set to time in your region.

## Keymap
`KEYMAP` variable is used to specify which keymap should be used. Default value is empty, which means that system will use default keymap provided by kernel.

Example:
```conf
KEYMAP=us
```

## Services
`SERVICES` variable is used to define services, which will be started, when system is booting, and stopped, when system is shutting down.

Example:
```conf
SERVICES="udevd dbus network"
```