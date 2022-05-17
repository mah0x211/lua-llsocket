# llsocket.msghdr

defined in [llsocket.msghdr](../src/msghdr.c).

```lua
local msghdr = require('llsocket').msghdr
```


## mh = msghdr.new()

create a `llsocket.msghdr` object.

**Returns**

- `mh:llsocket.msghdr`: `llsocket.msghdr` object.


## ai = mh:name( [ai] )

get the address-info, or change it to specified address-info. if argument is a nil, the associated address-info will be removed.

**Parameters**

- `ai:llsocket.addrinfo`: [llsocket.addrinfo](addrinfo.md) object.

**Returns**

- `ai:addrinfo`: previous [llsocket.addrinfo](addrinfo.md) object.


## iov = mh:iov( [iov] )

get the iovec, or change it to specified iovec. if argument is nil, the associated iovec will be removed.

**Parameters**

- `iov:iovec`: [iovec](https://github.com/mah0x211/lua-iovec) object.

**Returns**

- `iov:iovec`: previous [iovec](https://github.com/mah0x211/lua-iovec) object.


## cmsgs = mh:control( [cmsgs] )

get the cmsghdrs, or change it to specified cmsghdr. if argument is nil, the associated cmsghdrs will be removed.

**Parameters**

- `cmsgs:llsocket.cmsghdrs`: [llsocket.cmsghdrs](cmsghdrs.md) object.

**Returns**

- `cmsgs:llsocket.cmsghdrs`: previous [llsocket.cmsghdrs](cmsghdrs.md) object.


## flags = mh:flags()

get the flags.

**Returns**

- `flag:...`: [MSG_* flags](constants.md#msg_-flags) constants.

