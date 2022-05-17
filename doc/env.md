# llsocket.env

defined in [llsocket.env](../src/env.c).

```lua
local env = require('llsocket').env
```

`llsocket.env` module is the table that contains the following fields.

- `os:string`: one of the following strings:
  - `apple`, `dragonfly`, `freebsd`, `netbsd`, `openbsd`, `linux`, or `unknown`.
- `unixpath_max:integer`: maximum length of a pathname for unix domain socket.
- `somaxconn:integer`: maximum queue length specifiable by `listen`.

