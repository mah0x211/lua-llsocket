# llsocket.cmsghdr

defined in [llsocket.cmsghdr](../src/cmsghdr.c).

```lua
local cmsghdr = require('llsocket').cmsghdr
```


## cmsg = cmsghdr.new( level, type, data )

create a `llsocket.cmsghdr` object.

**Parameters**

- `level:integer`: socket option level - [Socket Option Levels](constants.md#socket-option-levels).
- `type:integer`: socket-level control message type - [Socket-level Control Message Types](constants.md#socket-level-control-message-types).
- `data:string`: ancillary data.

**Returns**

- `cmsg:llsocket.cmsghdr`: `llsocket.cmsghdr` object.


## cmsg = cmsghdr.rights( fd [, ...] )

create a  `llsocket.cmsghdr` object with file descriptors.

**Parameters**

- `fd:..`: file descriptors.

**Returns**

- `cmsg:llsocket.cmsghdr`: `llsocket.cmsghdr` object.


## level = cmsg:level()

get a socket option level.

**Returns**

- `level:integer`: socket option level - [Socket Option Levels](constants.md#socket-option-levels).


## type = cmsg:type()

get a socket-level control message type.

**Returns**

- `type:integer`: socket-level control message type - [Socket-level Control Message Types](constants.md#socket-level-control-message-types).


## ... = cmsg:data()

get ancillary data.

**Returns**

- `...`: ancillary data, or file descriptors if protocol-specific type socket.

