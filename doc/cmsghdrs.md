# llsocket.cmsghdrs

defined in [llsocket.cmsghdrs](../src/cmsghdrs.c).

```lua
local cmsghdrs = require('llsocket').cmsghdrs
```


## cmsgs = cmsghdrs.new()

create a `llsocket.cmsghdrs` object.

**Returns**

- `cmsgs:llsocket.cmsghdrs`: `llsocket.cmsghdrs` object.


## cmsgs:push( cmsg )

push `cmsg` into `cmsgs`.

**Parameters**

- `cmsg:llsocket.cmsghdr`: [llsocket.cmsghdr](cmsghdr.md) object.


## cmsg = cmsgs:shift()

remove the first `cmsghdr` from `cmsghdrs` and returns the removed object.

**Returns**

- `cmsg:llsocket.cmsghdr`: [llsocket.cmsghdr](cmsghdr.md) object.



