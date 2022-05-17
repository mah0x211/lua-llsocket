# llsocket.device

defined in [llsocket.device](../src/device.c).

```lua
local device = require('llsocket').device
```

## tbl, err = device.getifaddrs()

get interface addresses.

**Returns**

- `tbl:table`: key-value pairs of interface addresses.
- `err:error`: error object.

