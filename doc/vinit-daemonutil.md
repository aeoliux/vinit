# vinit-daemonutil
Tool for managing daemons.

## Usage
Usage of vinit-daemonutil:
```
vinit-daemonutil [ -P pidfile ] [ -n name ] [ -w ] [ -N ] [ -h ] [ -S command || -X || -C ]
```

Options:
- `-P pidfile` -> Specify path for file which contains process' PID,
- `-n name` -> Specify name for service (default is first argument of command),
- `-w` -> Wait until process finishes (default is forking),
- `-N` -> Don't create pidfile,
- `-h` -> Display help message.

Operation:
- `-S command` -> Starts process,
- `-X` -> Kills process,
- `-C` -> Checks if process is running. Unless it's running, deletes pidfile.

## Example usage:
### dbus
Starting dbus:
```
vinit-daemonutil -n dbus -S dbus-daemon -- --system --nofork
```
Command's arguments have to be placed after `--`.

Stopping dbus:
```
vinit-daemonutil -X dbus
```

Checking if dbus is running:
```
vinit-daemonutil -C dbus
```
Returns 0, if process is running, unless, returns 1.

### lightdm
Starting lightdm:
```
vinit-daemonutil -S lightdm
```
If `-n name` hasn't been specified, daemon name is taken from first command argument indexed by 0 (`lightdm` in this case).

Stopping and checking are the same as with dbus case, but with changed `dbus` to `lightdm`