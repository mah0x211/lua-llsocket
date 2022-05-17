# llsocket.addrinfo

defined in [llsocket.addrinfo](../src/addrinfo.c).

```lua
local addrinfo = require('llsocket').addrinfo
```

## ai, err = addrinfo.unix( pathname [, socktype [, protocol [, flag, ...]]] )

create a new `llsocket.addrinfo` object of `AF_UNIX`.

**Parameters**

- `pathname:string`: pathname of unix domain socket.
- `socktype:integer` [SOCK_* types](constants.md#sock_-types) constants.
- `protocol:integer`: [IPROTO_* types](constants.md#ipproto_-types) constants.
- `flags:...`: [AI_* flags](constants.md#ai-flags) constants.

**Returns**

- `ai:llsocket.addrinfo`: `llsocket.addrinfo` object.
- `err:error`: error object.


## ai, err = addrinfo.inet( [addr [, port [, socktype [, protocol [, flag, ...]]]]] )

create a new `llsocket.addrinfo` object of `AF_INET`.

**Parameters**

- `addr:string`: ipv4 host addresss.
- `port:integer`: port number.
- `socktype:integer` [SOCK_* types](constants.md#sock_-types) constants.
- `protocol:integer`: [IPROTO_* types](constants.md#ipproto_-types) constants.
- `flags:...`: [AI_* flags](constants.md#ai_-flags) constants.

**Returns**

- `ai:llsocket.addrinfo`: `llsocket.addrinfo` object.
- `err:error`: error object.


## ai, err = addrinfo.inet6( [addr [, port [, socktype [, protocol [, flag, ...]]]]] )

create a new addrinfo instance of `AF_INET6`.

**Parameters**

- `addr:string`: ipv6 host addresss.
- `port:integer`: port number.
- `socktype:integer` [SOCK_* types](constants.md#sock_-types) constants.
- `protocol:integer`: [IPROTO_* types](constants.md#ipproto_-types) constants.
- `flags:...`: [AI_* flags](constants.md#ai_-flags) constants.

**Returns**

- `ai:llsocket.addrinfo`: `llsocket.addrinfo` object.
- `err:error`: error object.


## ais, err = addrinfo.getaddrinfo( [host [, port [, family [, socktype [, protocol [, flag, ...]]]]]] )

get a list of address info of tcp stream socket.

**Parameters**

- `host:string`: host string.
- `port:string`: either a decimal port number or a service name listed in `services(5)`.
- `family:integer`: [AF_* types](constants.md#af_-types) constants.
- `socktype:integer` [SOCK_* types](constants.md#sock_-types) constants.
- `protocol:integer`: [IPROTO_* types](constants.md#ipproto_-types) constants.
- `flags:...`: [AI_* flags](constants.md#ai_-flags) constants.

**Returns**

- `ais:llsocket.addrinfo[]`: list of `llsocket.addrinfo` object.
- `err:error`: error object.


## nameinfo, err = ai:getnameinfo( [flag, ...] )

get hostname and service name.

**Parameters**

- `flag:...`: [NI_* flags](constants.md#ni_-flags) constants.

**Returns**

- `nameinfo:table`: name info table.
    - `host:string`: hostname.
    - `serv:string`: service name.
- `err:error`: error object.


## family = ai:family()

get a address family type.

**Returns**

- `family:integer`: [AF_* types](constants.md#af_-types) constants.


## socktype = ai:socktype()

get a socket type.

**Returns**

- `socktype:integer`: [SOCK_* type](constants.md#sock_-types) constants.


## protocol = ai:protocol()

get a protocol type.

**Returns**

- `protocol:integer`: [IPPROTO_* type](constants.md#ipproto_-types) constants.


## canonname = ai:canonname()

get a canonical name for service location.

**Returns**

- `canonname:string`: canonical name for service location.


## port = ai:port()

get a port number.

**Returns**

- `port:integer`: port number.


## addr = ai:addr()

get an address.

**Returns**

- `addr:string`: address or pathname string.

