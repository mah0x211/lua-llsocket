
# llsocket.socket

defined in [llsocket.socket](../src/socket.c).

```lua
local socket = require('llsocket').socket
```

## socks, err = socket.pair( socktype [, protocol [, nonblock]] )

create the pair of connected `llsocket.socket` objects.

**Parameters**

- `socktype:integer` [SOCK_* types](constants.md#sock_-types) constants.
- `protocol:integer`: [IPROTO_* types](constants.md#ipproto_-types) constants.
- `nonblock:boolean`: enable the `O_NONBLOCK` flag.

**Returns**

- `socks:table`: pair of connected sockets.
    - `1:llsocket.socket`: `llsocket.socket` object.
    - `2:llsocket.socket`: `llsocket.socket` object.
- `err:error`: error object.


## sock, err = socket.wrap( fd [, nonblock] )

create a `llsocket.socket` object from specified socket file descriptor.

**Parameters**

- `fd:integer`: socket file descriptor.
- `nonblock:boolean`: enable the `O_NONBLOCK` flag.

**Returns**

- `sock:llsocket.socket`: `llsocket.socket` object.
- `err:error`: error object.


## sock, err = socket.new( family, socktype, [protocol [, nonblock]] )

create a `llsocket.socket` object.

**Parameters**

- `family:integer`: [AF_* types](constants.md#af_-types) constants.
- `socktype:integer` [SOCK_* types](constants.md#sock_-types) constants.
- `protocol:integer`: [IPROTO_* types](constants.md#ipproto_-types) constants.
- `nonblock:boolean`: enable the `O_NONBLOCK` flag.

**Returns**

- `sock:llsocket.socket`: `llsocket.socket` object.
- `err:error`: error object.


## ok, err = socket.shutdown( fd, [flag] )

shut down part of a full-duplex connection.

**Parameters**

- `fd:integer`: socket file descriptor.
- `flag:integer`: [SHUT_* flag](constants.md#shut_-flags) constants. (default `SHUT_RDWR`)

**Returns**

- `ok:boolean`: `true` on success.
- `err:error`: error object.


## ok, err = socket.close( fd, [flag] )

close a file descriptor.

**Parameters**

- `fd:integer`: socket file descriptor.
- `flag:integer`: [SHUT_* flag](constants.md#shut_-flags) constants.

**Returns**

- `ok:boolean`: `true` on success.
- `err:error`: error object.


## gcfn, err = socket:addgcfn( errfunc, func, ... )

add the user-defined function that will be called when socket is closed or unwrapped.

**NOTE**

user-defined functions are called in the order in which they were last added.

**Parameters**

- `errfunc:function`: error function that will be called when user-defined function is failed on error.
- `func:function`: user-defined function.
- `...:...`: arguments for user-defined function.

**Returns**

- `gcfn:llsocket.gcfn`: An object to be passed to the [socket:delgcfn()](#ok--socketdelgcfn-gcfn-) method when a user-defined function needs to be deleted.
- `err:error`: error object.


## ok = socket:delgcfn( gcfn )

delete a user-defined function.

**Parameters**

- `gcfn:llsocket.gcfn`: an object returned by [socket:addgcfn()](#gcfn-err--socketaddgcfn-errfunc-func--) method.

**Returns**

- `ok:boolean`: `true` on success.


## fd = socket:unwrap()

get a socket file descriptor, and disable a `sock`.

**Returns**

- `fd:integer`: socket file descriptor.


## sock, err = socket:dup()

duplicate a `sock`.

**Returns**

- `sock:llsocket.socket`: `llsocket.socket` object.
- `err:error`: error object.


## fd = socket:fd()

get a socket file descriptor.

**Returns**

- `fd:integer`: socket file descriptor.


## family = socket:family()

get a address family type.

**Returns**

- `family:integer`: [AF_* types](constants.md#af_-types) constants.


## socktype = socket:socktype()

get a socket type.

**Returns**

- `socktype:integer`: [SOCK_* type](constants.md#sock_-types) constants.


## protocol = socket:protocol()

get a protocol type.

**Returns**

- `protocol:integer`: [IPPROTO_* type](constants.md#ipproto_-types) constants.


## ok, err = socket:bind( ai )

bind a `sock` to an address `ai`.

**Parameters**

- `ai:llsocket.addrinfo`: [llsocket.addrinfo](addrinfo.md) object.

**Returns**

- `ok:boolean`: `true` on success.
- `err:error`: error object.


## ok, err, again = socket:connect( ai )

initiate a new connection.

**Parameters**

- `ai:llsocket.addrinfo`: [llsocket.addrinfo](addrinfo.md).

**Returns**

- `ok:boolean`: `true` on success.
    - if `err.type` is `errno.EINPROGRESS` or `errno.EALREADY`, you must check the `errno` by [socket:error()](#soerr-err--socketerror) after a while.
- `err:error`: error object.
- `again:boolean`: `true` if errno is `EAGAIN` or `ETIMEDOUT`.

**Example**

```lua
local llsocket = require('llsocket')
local socket = llsocket.socket
local addrinfo = llsocket.addrinfo

local ais = addrinfo.getaddrinfo('example.com', '80')
for _, ai in ipairs(ais) do
    -- create socket
    local c = socket.new(ai:family(), ai:socktype())
    c:nonblock(true)

    -- connect
    local ok, err, again = c:connect(ai)
    while again then
        ok, err, again = c:connect(ai)
    end
    
    if not ok then
        -- failed to connect
        error(err)
    elseif err then
        print(err) -- in method 'connect': [EINPROGRESS:36][connect] Operation now in progress
        print(c:error())
    end
end
```

## ok, err = socket:shutdown( [flag] )

shut down part of a full-duplex connection.

**Parameters**

- `flag:integer`: [SHUT_* flag](constants.md#shut_-flags) constants. (default `SHUT_RDWR`)

**Returns**

- `ok:boolean`: `true` on success.
- `err:error`: error object.


## ok, err = socket:close( [flag] )

close a file descriptor.

**Parameters**

- `flag:integer`: [SHUT_* flag](constants.md#shut_-flags) constants.

**Returns**

- `ok:boolean`: `true` on success.
- `err:error`: error object.


## ok, err = socket:listen( [backlog] )

listen for connections.

**Parameters**

- `backlog:integer`: the maximum length for the queue of pending connections.

**Returns**

- `ok:boolean`: `true` on success.
- `err:error`: error object.


## sock, err, again, ai = socket:accept( [with_ai] )

accept a connection.

**Parameters**

- `with_ai:boolean`: `true` to receive socket with address info.

**Returns**

- `sock:llsocket.socket`: `llsocket.socket` object.
- `err:error`: error object.
- `again:boolean`: `true` if `errno` is `EAGAIN`, `EWOULDBLOCK`, `EINTR` or `ECONNABORTED`.
- `ai:llsocket.addrinfo`: [llsocket.addrinfo](addrinfo.md) object.


## fd, err, again = socket:acceptfd()

accept a connection.

**Returns**

- `fd:integer`: socket file descriptor.
- `err:error`: error object.
- `again:boolean`: `true` if `errno` is `EAGAIN`, `EWOULDBLOCK`, `EINTR` or `ECONNABORTED`.


## ok, err, timeout = socket:sendable( [sec [, exception]] )

wait until socket can be sendable within specified timeout seconds.

**Parameters**

- `sec:number`: timeout seconds. (default `0`)
- `exception:boolean`: enable exception waiting. (default `false`)

**Returns**

- `ok:boolean`: if `true`, socket is ready to send.
- `err:error`: error object.
- `timeout:boolean`: timed-out.


## len, err, again = socket:write( msg )

write a message.

**Parameters**

- `msg:string`: message string.

**Returns**

- `len:integer`: the number of bytes written.
- `err:error`: error object.
- `again:boolean`: `true` if len != #msg, or `errno` is `EAGAIN`, `EWOULDBLOCK` or `EINTR.



## len, err, again = socket:send( msg [, flag, ...] )

send a message.

**Parameters**

- `msg:string`: message string.
- `flag:...`: [MSG_* flags](constants.md#msg_-flags) constants.

**Returns**

- `len:integer`: the number of bytes sent.
- `err:error`: error object.
- `again:boolean`: `true` if len != #msg, or `errno` is `EAGAIN`, `EWOULDBLOCK` or `EINTR.


## len, err, again = socket:sendto( msg, ai [, flag, ...] )

send a message to specified destination address.

**Parameters**

- `msg:string`: message string.
- `ai:llsocket.addrinfo`: [llsocket.addrinfo](addrinfo.md) object.
- `flag:...`: [MSG_* flags](constants.md#msg_-flags) constants.

**Returns**

- `len:integer`: the number of bytes sent.
- `err:error`: error object.
- `again:boolean`: `true` if len != #msg, or `errno` is `EAGAIN`, `EWOULDBLOCK` or `EINTR`.


## len, err, again = socket:sendfd( fd, [ai, [flag, ...]] )

send file descriptors along unix domain sockets.

**Parameters**

- `fd:integer`: file descriptor.
- `ai:llsocket.addrinfo`: [llsocket.addrinfo](addrinfo.md) object.
- `flag:...`: [MSG_* flags](constants.md#msg_-flags) constants.

**Returns**

- `len:integer`: the number of bytes sent (always zero).
- `err:error`: error object.
- `again:boolean`: `true` if `errno` is `EAGAIN`, `EWOULDBLOCK` or `EINTR`.


## len, err, again = socket:sendmsg( mh [, flag, ...] )

send multiple messages including auxiliary data at once.

**Parameters**

- `mh:llsocket.msghdr`: [llsocket.msghdr](msghdr.md) object.
- `flag:...`: [MSG_* flags](constants.md#msg_-flags) constants.

**Returns**

- `len:integer`: the number of bytes sent.
- `err:error`: error object.
- `again:boolean`: `true` if len != `mh:bytes()`, or `errno` is `EAGAIN`, `EWOULDBLOCK` or `EINTR`.


## len, err, again = socket:sendfile( fd, bytes [, offset] )

send a file.

**Parameters**

- `fd:integer|file`: file descriptor or file handle.
- `bytes:integer`: how many bytes of the file should be sent.
- `offset:integer`: where to begin in the file.

**Returns**

- `len:integer`: the number of bytes sent.
- `err:error`: error object.
- `again:boolean`: `true` if len != #bytes, or `errno` is `EAGAIN` or `EINTR`.


## ok, err, timeout = socket:recvable( [sec [, exception]] )

wait until the socket can be receivable within specified timeout seconds.

**Parameters**

- `sec:integer`: timeout seconds. (default `0`)
- `exception:boolean`: enable exception waiting. (default `false`)

**Returns**

- `ok:boolean`: if `true`, socket is ready to receive.
- `err:error`: error object.
- `timeout:boolean`: timed-out.


## msg, err, again = socket:read( [bufsize] )

read a message.

**Parameters**

- `bufsize:integer`: working buffer size of receive operation.

**Returns**

- `msg:string`: message string.
- `err:error`: error object.
- `again:boolean`: `true` if `errno` is `EAGAIN`, `EWOULDBLOCK` or `EINTR`.

**NOTE:** all return values will be nil if the number of bytes received is `0` and socket type is not `SOCK_DGRAM` and `SOCK_RAW`.


## msg, err, again = socket:recv( [bufsize [, flag, ...]] )

receive a message.

**Parameters**

- `bufsize:integer`: working buffer size of receive operation.
- `flag:...`: [MSG_* flags](constants.md#msg_-flags) constants.

**Returns**

- `msg:string`: received message string.
- `err:error`: error object.
- `again:boolean`: `true` if `errno` is `EAGAIN`, `EWOULDBLOCK` or `EINTR`.

**NOTE:** all return values will be nil if the number of bytes received is `0` and socket type is not `SOCK_DGRAM` and `SOCK_RAW`.


## msg, err, again, ai = socket:recvfrom( [bufsize [, flag, ...]] )

receive message and address info.

**Parameters**

- `bufsize:integer`: working buffer size of receive operation.
- `flag:...`: [MSG_* flags](constants.md#msg_-flags) constants.

**Returns**

- `msg:string`: received message string.
- `err:error`: error object.
- `again:boolean`: `true` if `errno` is `EAGAIN`, `EWOULDBLOCK` or `EINTR`.
- `ai:llsocket.addrinfo`: [llsocket.addrinfo](addrinfo.md) object.

**NOTE:** all return values will be nil if the number of bytes received is `0` and socket type is not `SOCK_DGRAM` and `SOCK_RAW`.


## fd, err, again = socket:recvfd( [flag, ...] )

receive file descriptors along unix domain sockets.

**Parameters**

- `flag:...`: [MSG_* flags](constants.md#msg_-flags) constants.

**Returns**

- `fd:integer`: file descriptor.
- `err:error`: error object.
- `again:boolean`: `true` either if `errno` is `EAGAIN`, `EWOULDBLOCK` or `EINTR`.

**NOTE:** all return values will be nil if the number of bytes received is `0` and socket type is not `SOCK_DGRAM` and `SOCK_RAW`.


## len, err, again = socket:recvmsg( mh [, flag, ...] )

receive multiple messages including auxiliary data at once.

**Parameters**

- `mh:llsocket.msghdr`: [llsocket.msghdr](msghdr.md) object.
- `flag:...`: [MSG_* flags](constants.md#msg_-flags) constants.

**Returns**

- `len:integer`: the number of bytes received.
- `err:error`: error object.
- `again:boolean`: `true` if len != `mh:bytes()`, or `errno` is `EAGAIN`, `EWOULDBLOCK` or `EINTR`.

**NOTE:** all return values will be nil if the number of bytes received is `0` and socket type is not `SOCK_DGRAM` and `SOCK_RAW`.


## bool, err = socket:atmark()

determine whether socket is at out-of-band mark.

**Returns**

- `bool:boolean`: `true` if the socket is at the out-of-band mark.
- `err:error`: error object.


## ai, err = socket:getsockname()

get socket name.

**Returns**

- `ai:llsocket.addrinfo`: [llsocket.addrinfo](addrinfo.md) object.
- `err:error`: error object.


## ai, err = socket:getpeername()

get address of connected peer.

**Returns**: same as [socket:getsockname()](#ai-err--socketgetsockname).


## enable, err = socket:cloexec( [enable] )

determine whether the `FD_CLOEXEC` flag enabled, or change the state to an argument value.

**Parameters**

- `enable:boolean`: to enable or disable the `FD_CLOEXEC` flag.

**Returns**

- `enable:boolean`: the state before changing the `FD_CLOEXEC` flag.
- `err:error`: error object.


## enable, err = socket:nonblock( [enable] )

determine whether the `O_NONBLOCK` flag enabled, or change the state to an argument value.

**Parameters**

- `enable:boolean`: to enable or disable the `O_NONBLOCK` flag.

**Returns**

- `enable:boolean`: the state before changing the `O_NONBLOCK` flag.
- `err:error`: error object.


## soerr, err = socket:error()

get pending socket error status and clear it.

**Returns**

- `soerr:error`: error object of the socket.
- `err:error`: error object.


## enabled, err = socket:acceptconn()

determine whether the `SO_ACCEPTCONN` flag enabled.

**Returns**

- `enabled:boolean`: the state of the `SO_ACCEPTCONN` flag.
- `err:error`: error object.


## enable, err = socket:tcpnodelay( [enable] )

determine whether the `TCP_NODELAY` flag enabled, or change the state to an argument value.

**Parameters**

- `enable:boolean`: to enable or disable the `TCP_NODELAY` flag.

**Returns**

- `enable:boolean`: the state before changing the `TCP_NODELAY` flag.
- `err:error`: error object.


## sec, err = socket:tcpkeepintvl( [sec] )

get the `TCP_KEEPINTVL` value, or change that value to an argument value.

**Parameters**

- `sec:integer`: set the `TCP_KEEPINTVL` value.

**Returns**

- `sec:integer`: the value before changing the `TCP_KEEPINTVL`.
- `err:error`: error object.


## cnt, err = socket:tcpkeepcnt( [cnt] )

get the `TCP_KEEPCNT` value, or change that value to an argument value.

**Parameters**

- `cnt:integer`: set the `TCP_KEEPCNT` value.

**Returns**

- `sec:integer`: the value before changing the `TCP_KEEPCNT`.
- `err:error`: error object.


## sec, err = socket:tcpkeepalive( [sec] )

get the `TCP_KEEPALIVE` value, or set that value if argument passed.

**Parameters**

- `sec:integer`: set the `TCP_KEEPALIVE` value.

**Returns**

- `sec:integer`: the value before changing the `TCP_KEEPALIVE`.
- `err:error`: error object.


## enable, err = socket:tcpcork( [enable] )

determine whether the `TCP_CORK` flag enabled, or change the state to an argument value.

**Parameters**

- `enable:boolean`: to enable or disable the `TCP_CORK` flag.

**Returns**

- `enable:boolean`: the state before changing the `TCP_CORK` flag.
- `err:error`: error object.


## enable, err = socket:reuseport( [enable] )

determine whether the `SO_REUSEPORT` flag enabled, or change the state to an argument value.

**Parameters**

- `enable:boolean`: to enable or disable the `SO_REUSEPORT` flag.

**Returns**

- `enable:boolean`: the state before changing the `SO_REUSEADDR` flag.
- `err:error`: error object.


## enable, err = socket:reuseaddr( [enable] )

determine whether the `SO_REUSEADDR` flag enabled, or change the state to an argument value.

**Parameters**

- `enable:boolean`: to enable or disable the `SO_REUSEADDR` flag.

**Returns**

- `enable:boolean`: the state before changing the `SO_REUSEADDR` flag.
- `err:error`: error object.


## enable, err = socket:broadcast( [enable] )

determine whether the `SO_BROADCAST` flag enabled, or change the state to an argument value.

**Parameters**

- `enable:boolean`: to enable or disable the `SO_BROADCAST` flag.

**Returns**

- `enable:boolean`: the state before changing the `SO_BROADCAST` flag.
- `err:error`: error object.


## enable, err = socket:debug( [enable] )

determine whether the `SO_DEBUG` flag enabled, or change the state to an argument value.

**Parameters**

- `enable:boolean`: to enable or disable the `SO_DEBUG` flag.

**Returns**

- `enable:boolean`: the state before changing the `SO_DEBUG` flag.
- `err:error`: error object.


## enable, err = socket:keepalive( [enable] )

determine whether the `SO_KEEPALIVE` flag enabled, or change the state to an argument value.

**Parameters**

- `enable:boolean`: to enable or disable the `SO_KEEPALIVE` flag.

**Returns**

- `enable:boolean`: the state before changing the `SO_KEEPALIVE` flag.
- `err:error`: error object.


## enable, err = socket:oobinline( [enable] )

determine whether the `SO_OOBINLINE` flag enabled, or change the state to an argument value.

**Parameters**

- `enable:boolean`: to enable or disable the `SO_OOBINLINE` flag.

**Returns**

- `enable:boolean`: the state before changing the `SO_OOBINLINE` flag.
- `err:error`: error object.


## enable, err = socket:dontroute( [enable] )

determine whether the `SO_DONTROUTE` flag enabled, or change the state to an argument value.

**Parameters**

- `enable:boolean`: to enable or disable the `SO_DONTROUTE` flag.

**Returns**

- `enable:boolean`: the state before changing the `SO_DONTROUTE` flag.
- `err:error`: error object.


## enable, err = socket:timestamp( [enable] )

determine whether the `SO_TIMESTAMP` flag enabled, or change the state to an argument value.

**Parameters**

- `enable:boolean`: to enable or disable the `SO_TIMESTAMP` flag.

**Returns**

- `enable:boolean`: the state before changing the `SO_TIMESTAMP` flag.
- `err:error`: error object.


## sz, err = socket:rcvbuf( [sz] )

get the `SO_RCVBUF` value, or change that value to an argument value.

**Parameters**

- `sz:integer`: set the `SO_RCVBUF` value.

**Returns**

- `sz:integer`: the value before changing the `SO_RCVBUF`.
- `err:error`: error object.


## sz, err = socket:rcvlowat( [sz] )

get the `SO_RCVLOWAT` value, or change that value to an argument value.

**Parameters**

- `sz:integer`: set the `SO_RCVLOWAT` value.

**Returns**

- `sz:integer`: the value before changing the `SO_RCVLOWAT`.
- `err:error`: error object.


## sz, err = socket:sndbuf( [sz] )

get the `SO_SNDBUF` value, or change that value to an argument value.

**Parameters**

- `sz:integer`: set the `SO_SNDBUF` value.

**Returns**

- `sz:integer`: the value before changing the `SO_SNDBUF`.
- `err:error`: error object.


## sz, err = socket:sndlowat( [sz] )

get the `SO_SNDLOWAT` value, or change that value to an argument value.

**Parameters**

- `sz:integer`: set the `SO_SNDLOWAT` value.

**Returns**

- `sz:integer`: the value before changing the `SO_SNDLOWAT`.
- `err:error`: error object.


## sec, err = socket:rcvtimeo( [sec] )

get the `SO_RCVTIMEO` value, or change that value to an argument value.

**Parameters**

- `sec:number`: set the `SO_RCVTIMEO` value.

**Returns**

- `sec:number`: the value before changing the `SO_RCVTIMEO`.
- `err:error`: error object.


## sec, err = socket:sndtimeo( [sec] )

get the `SO_SNDTIMEO` value, or change that value to an argument value.

**Parameters**

- `sec:number`: set the `SO_SNDTIMEO` value.

**Returns**

- `sec:number`: the value before changing the `SO_SNDTIMEO`.
- `err:error`: error object.


## sec, err = socket:linger( [sec] )

get the `SO_LINGER` value, or change that value to an argument value.

**Parameters**

- `sec:integer`: if `sec >= 0` then enable `SO_LINGER` option, or else disabled this option.

**Returns**

- `sec:integer`: the value before changing the `SO_LINGER`.
- `err:error`: error object.


## enable, err = socket:mcastloop( [enable] )

determine whether the `IP_MULTICAST_LOOP` flag enabled, or change the state to an argument value.

**Parameters**

- `enable:boolean`: to enable or disable the `IP_MULTICAST_LOOP` flag.

**Returns**

- `enable:boolean`: the state before changing the `IP_MULTICAST_LOOP` flag.
- `err:error`: error object.


## ttl, err = socket:mcastttl( [ttl] )

get the `IP_MULTICAST_TTL` value, or change that value to an argument value.

**Parameters**

- `ttl:integer`: set the `IP_MULTICAST_TTL` value.

**Returns**

- `sec:integer`: the value before changing the `IP_MULTICAST_TTL`.
- `err:error`: error object.


## ifname, err = socket:mcastif( [ifname] )

get the `IP_MULTICAST_IF` value, or change that value to an argument value.

**Parameters**

- `ifname:string`: set the `IP_MULTICAST_IF` value.

**Returns**

- `ifname:string`: the value before changing the `IP_MULTICAST_IF`.
- `err:error`: error object.


## ok, err = socket:mcastjoin( mcaddr [, ifname] )

set the `IP_ADD_MEMBERSHIP` or `IPV6_JOIN_GROUP` (if IPv6) value.

**Parameters**

- `mcaddr:llsocket.addrinfo`: multicast group address.
- `ifname:string`: interface name.

**Returns**

- `ok:boolean`: `true` on success.
- `err:error`: error object.


## ok, err = socket:mcastleave( mcaddr [, ifname] )

set the `IP_DROP_MEMBERSHIP` or `IPV6_LEAVE_GROUP` (if IPv6) value.

**Parameters**

- `mcaddr:llsocket.addrinfo`: multicast group address.
- `ifname:string`: interface name.

**Returns**

- `ok:boolean`: `true` on success.
- `err:error`: error object.


## ok, err = socket:mcastjoinsrc( mcaddr, srcaddr [, ifname] )

set the `IP_ADD_SOURCE_MEMBERSHIP` or `MCAST_JOIN_SOURCE_GROUP` (if IPv6) value.

**Parameters**

- `mcaddr:llsocket.addrinfo`: multicast group address.
- `srcaddr:llsocket.addrinfo`: multicast source address.
- `ifname:string`: interface name.

**Returns**

- `ok:boolean`: `true` on success.
- `err:error`: error object.


## ok, err = socket:mcastleavesrc( mcaddr, srcaddr [, ifname] )

set the `IP_DROP_SOURCE_MEMBERSHIP` or `MCAST_LEAVE_SOURCE_GROUP` (if IPv6) value.

**Parameters**

- `mcaddr:llsocket.addrinfo`: multicast group address.
- `srcaddr:llsocket.addrinfo`: multicast source address.
- `ifname:string`: interface name.

**Returns**

- `ok:boolean`: `true` on success.
- `err:error`: error object.


## ok, err = socket:mcastblocksrc( mcaddr, srcaddr [, ifname] )

set the `IP_BLOCK_SOURCE` or `MCAST_BLOCK_SOURCE` (if IPv6) value.

**Parameters**

- `mcaddr:llsocket.addrinfo`: multicast group address.
- `srcaddr:llsocket.addrinfo`: multicast source address.
- `ifname:string`: interface name.

**Returns**

- `ok:boolean`: `true` on success.
- `err:error`: error object.


## ok, err = socket:mcastunblocksrc( mcaddr, srcaddr [, ifname] )

set the `IP_UNBLOCK_SOURCE` or `MCAST_UNBLOCK_SOURCE` (if IPv6) value.

**Parameters**

- `mcaddr:llsocket.addrinfo`: multicast group address.
- `srcaddr:llsocket.addrinfo`: multicast source address.
- `ifname:string`: interface name.

**Returns**

- `ok:boolean`: `true` on success.
- `err:error`: error object.

