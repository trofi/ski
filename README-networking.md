# Ski networking #

Ski supports two ways of networking:

1. The original _Socket Filtering_ or _Berkeley Packet Filter_ (BPF) based networking that requires a distinct physical network interface for communication between the Ski instance and the host and network. Always enabled, unless explicitly disabled by using `--disable-netdev` or `--enable-tuntap` in the configure step.

2. The newer TUN/TAP based networking that can work with a virtual TAP device, similar to QEMU/KVM. Only enabled with `--enable-tuntap` in the configure step.


## BPF based networking ##

Ski auto-ups the respective physical device on the host before use. The physical device on the host mustn't have an IP address configured. An IP address is configured for the emulated interface in the Ski instance though. For this type of networking Ski has to run under the root account or needs the [capabilities](https://man7.org/linux/man-pages/man7/capabilities.7.html) `CAP_NET_ADMIN` and `CAP_NET_RAW`. Set these with `setcap 'cap_net_raw=ep cap_net_admin=ep' ski`. In the latter case Ski can run under an ordinary user account.

### Example ###

```
 network

+------------|----------------------------|----------------+
| host       |                            |                |
|            |                            |                |
|  +---------+-----------+   +------------+-------------+  |
|  | host main interface |   | other physical interface |  |
|  +---------------------+   +------------+-------------+  |
|                                         |                |
|                               +---------+---------+      |
|                               |         |         |      |
|                               | +-------+-------+ |      |
|                               | | Ski interface | |      |
|                               | +---------------+ |      |
|                               |                   |      |
|                               | Ski instance      |      |
|                               +-------------------+      |
|                                                          |
+----------------------------------------------------------+

```

| What         | IP address/metmask |
| ------------ | ------------------ |
| network      | 172.16.0.0/16      |
| host         | 172.16.0.2/16      |
| Ski instance | 172.16.0.3/16      |


## TUN/TAP networking ##

When the used TAP device on the host was not pre-created before launching the Ski instance using it, a TAP interface is created automatically (requires Ski to run as root or have `CAP_NET_ADMIN` set). When pre-created before launch of Ski, Ski can run and make use of it as ordinary user w/o the need for any capabilities.

To allow network communication the usual setup includes a host-local bridge which bridges the main network interface of the host with the TAP device that is used by a Ski instance. The bridge has an IP address configured for communications with other networked hosts. The bridged interfaces don't have an IP address configured on the host. An IP address is configured for the emulated interface in the Ski instance though, which in this case is the other end of the TAP device on the host.

### Example ###

```
 network

+------------|-----------------+
| host       |                 |
|            |                 |
|  +---------+-----------+     |
|  | host main interface |     |
|  +-----+--------+------+     |
|        | bridge |            |
|  +-----+--------+------+     |
|  | TAP device          |     |
|  +---------+-----------+     |
|            |                 |
|            |                 |
|  +---------+---------+       |
|  |         |         |       |
|  | +-------+-------+ |       |
|  | | Ski interface | |       |
|  | +---------------+ |       |
|  |                   |       |
|  | Ski instance      |       |
|  +-------------------+       |
|                              |
+------------------------------+

```

| What         | IP address/metmask |
| ------------ | ------------------ |
| network      | 172.16.0.0/16      |
| host         | 172.16.0.2/16      |
| Ski instance | 172.16.0.3/16      |

> **NOTE:** In the TUN/TAP case make sure that the TAP device on the host is always up before it is used inside Ski. Otherwise the Ski instance will crash on first use. Ski does not auto-up the TAP device on the host in this case, as it would then need to always run as root or with the respective capabilities to do that and then loose the ability to be ran under an ordinary user account.
