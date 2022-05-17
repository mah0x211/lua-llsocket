# Error Handling

the llsocket functions/methods are return the error object created by https://github.com/mah0x211/lua-errno module.

```lua
local errno = require('errno')
local llsocket = require('llsocket')
local addrinfo = llsocket.addrinfo

local ai, err = addrinfo.inet('127.0.0.1.1', 8080, llsocket.SOCK_STREAM)
print(ai, err.type, err.type == errno.EAFNOSUPPORT) -- nil EAFNOSUPPORT: 0x7fad6f6060e8 true
```
