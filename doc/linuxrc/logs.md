# vinit linuxrc logs
vinit linuxrc uses [vinit-logger](../vinit-logger.md) to store daemons output.

## Clearing vinit linuxrc logs
If logs takes much space in your partition, try deleting it by deleting `/var/log/services` folder:
```sh
# Run this command as root
rm -r /var/log/services
```
