Limiting the number of connections
----------------------------------


As a network-facing service, `sslh` can be attacked by very
simple denial-of-service attacks by creating a lot of
concurrent connections. Protecting against such attacks is
complicated from the server side. This attack will cause
`sslh` to create a large number of file handles, which can
cause exgagerated resource consumption.

This threat can be mitigated using several limitation
mechanisms. Keep in mind that limiting the number of
connections means that in case of an attack, the server
resources are protected, but this might be at the expense of
serving legitimate connections. In particular, in some cases
this might mean SSH is no longer available.

There are several mechanisms to limit the number of
connections:

- Use `ulimit` (see bash(3) or your shell's man page) to
limit the number of file descriptors.

Then, `sslh` provides several mechanisms to limit the number
of concurrent connections, which in turns limits the number
of file descriptors used.

Essentially there are two ways to do this:

- you can set `max_connections` per protocol, and `sslh`
will drop connections after probing if the count is
exceeded.  This should help in keeping SSH connections
available even if an attacker is stuffing other protocols.

- you can set `max_connections` for each `listen` entry. So
the `sslh` process for each port will limit the number of
concurrent connections to that port. This is similar to the
`udp_max_connections` setting, but for TCP.

`sslh-select` and `sslh-ev` support both limit types.

`sslh-fork` has an explicit design goal to be as simple as
possible, which makes it impossible to implement limits per
protocol (because protocol probing is performed after the
process has forked). Limits will only work for `listen`
entries.

As of sslh 2.2.5, this is an experimental feature and not
all use cases have been tested. If 2.2.5 does not exist yet,
you must be on the Git's head, so it is even more
experimtental! Do not hesitate to send feedback if you
notice inconsistent behaviour.
