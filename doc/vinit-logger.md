# vinit-logger
Command output logging tool.

## Usage
```
vinit-logger [ -L logfile ] command
```

Options:
- `-L logfile` -> Specify path for file, which logs vinit-logger stores in (default path is `/var/log/daemons.log`),
Command arguments have to be specified after `--`.

## Example usage
### Logging lightdm output
This command opens `/var/log/vinit-lightdm.log` for writing and stores output of command `lightdm --debug` in it:
```
vinit-logger -L /var/log/vinit-lightdm.log lightdm -- --debug
```
<br />
vinit-logger can be executed via vinit-daemonutil:
```
vinit-daemonutil -n lightdm -S vinit-logger -- -L /var/log/vinit-lightdm.log lightdm -- --debug
```