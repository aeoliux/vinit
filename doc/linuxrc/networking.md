# vinit linuxrc networking

## Some information
vinit linuxrc provides a `network` init script, which is used to initialize static or dynamic (dhcp) network connection.<br />
Settings for networking have to be placed in `/etc/rc.conf.d/network`.<br /><br />

`network` init script hasn't been supporting wireless network yet.<br />
Now `network` init script supports dynamic connection only using dhcpcd. Support for dhclient will be added in the future.<br />
To use static connection using `network` init script, you need to have:
- route
- ifconfig

## Installing `network` init script
To install `network` init script, you have to placed it in `/etc/rc` and make it executable.
1. Enter to vinit source code directory,
2. If you haven't already created a `/etc/rc.d` and `/etc/rc.conf.d` directory, execute:
```sh
install -dm755 /etc/rc.d /etc/rc.conf.d
```
3. Copy and make executable a `network` init script and example network configuration:
```sh
install -m755 linuxrc/rc.d/network /etc/rc.d
install -m755 linuxrc/rc.conf.d/network.conf /etc/rc.conf.d
```
4. Add `network` service. It should be placed AFTER udevd. Example `SERVICES` `rc.conf`
```conf
SERVICES="udevd network"
```

## Configuring network via `network` init script
In `/etc/rc.conf.d/network.conf` you can see commented (#) example network configuration.

### Configuring dynamic network connection on all network interfaces
At the end of `network.conf` add line:
```conf
USE_DHCP=1
```
and that's all you need.

### Per interfaces configuration (static and dynamic connection)
#### `interfaces`
If you want to configure each interface otherwise, define `interfaces` variable, which contains all interfaces that `network` should configure. In my case:
```conf
interfaces="eth0"
```
Also example `interfaces` is in `network.conf`. Check it.
<br />

#### `types`
Next thing is to define connection type (static or dynamic) which should be used on each interface. This options provides a `types` variable. Syntax for it:
```conf
types="<interface>-<option>"
```
Very important thing is that, all next options use this syntax `<interface>-<option>` (except `dns_servers`).<br />

For options we have:
- **dhcp** - dynamic network connection,
- **static** - static network connection,<br />

In my case:
- For dynamic connection:
```conf
types="eth0-dhcp"
```
- For static connection:
```conf
types="eth0-static"
```

If you manage to use the `dhcp` option, it is all what you have to do. When you run `network` service, it will run dhcp client on that interface and network will be configured.
Next sections are only for users, who want static network connection.

#### `ifconfig`
`ifconfig` is a variable which we define an IP address for an interface in. `ifconfig` uses the same syntax as `types` variable:
```conf
ifconfig="<interface>-<IP address>"
```

#### `netmasks`
`netmasks` is a variable for defining a netmask for an interface:
```conf
netmasks="<interface>-<netmask>"
```

#### `routes`
`routes` is last variable which is required to get static connection working. We define a routes for each interface in it:
```conf
routes="<interface>-<routes>"
```
<br />

`network` doesn't support an multiple routes for one interface, if you want to do it, try using `rc.local` with `route` command.

#### `dns_servers`
Variable for defining a DNS servers.
```conf
routes="<dns server> <dns server> ..."
```

### Example `network` configuration
Dynamic for all interfaces:
```conf
USE_DHCP=1
```

Dynamic for an `eth0` interface:
```conf
interfaces="eth0"
types="eth0-dhcp"
```

Static configuration for `eth0` interface:
```conf
interfaces="eth0"
types="eth0-static"
ifconfig="eth0-192.168.1.2"
netmasks="eth0-255.255.255.0"
routes="eth0-192.168.1.1"
dns_servers="192.168.1.1"
```
