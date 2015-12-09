# lua-llsocket

low-level socket module.

## Installation

```sh
luarocks install llsocket --from=http://mah0x211.github.io/rocks/
```


## Constants

these constants defined at the `llsocket.*`

**Use for `connect` and `bind`, `socketpair` API**

- SOCK_DGRAM
- SOCK_STREAM
- SOCK_SEQPACKET
- SOCK_RAW
- IPPROTO_IP
- IPPROTO_HOPOPTS
- IPPROTO_ICMP
- IPPROTO_IGMP
- IPPROTO_GGP
- IPPROTO_IPV4
- IPPROTO_IPIP
- IPPROTO_TCP
- IPPROTO_ST
- IPPROTO_EGP
- IPPROTO_PIGP
- IPPROTO_RCCMON
- IPPROTO_NVPII
- IPPROTO_PUP
- IPPROTO_ARGUS
- IPPROTO_EMCON
- IPPROTO_XNET
- IPPROTO_CHAOS
- IPPROTO_UDP
- IPPROTO_MUX
- IPPROTO_MEAS
- IPPROTO_HMP
- IPPROTO_PRM
- IPPROTO_IDP
- IPPROTO_TRUNK1
- IPPROTO_TRUNK2
- IPPROTO_LEAF1
- IPPROTO_LEAF2
- IPPROTO_RDP
- IPPROTO_IRTP
- IPPROTO_TP
- IPPROTO_BLT
- IPPROTO_NSP
- IPPROTO_INP
- IPPROTO_SEP
- IPPROTO_3PC
- IPPROTO_IDPR
- IPPROTO_XTP
- IPPROTO_DDP
- IPPROTO_CMTP
- IPPROTO_TPXX
- IPPROTO_IL
- IPPROTO_IPV6
- IPPROTO_SDRP
- IPPROTO_ROUTING
- IPPROTO_FRAGMENT
- IPPROTO_IDRP
- IPPROTO_RSVP
- IPPROTO_GRE
- IPPROTO_MHRP
- IPPROTO_BHA
- IPPROTO_ESP
- IPPROTO_AH
- IPPROTO_INLSP
- IPPROTO_SWIPE
- IPPROTO_NHRP
- IPPROTO_ICMPV6
- IPPROTO_NONE
- IPPROTO_DSTOPTS
- IPPROTO_AHIP
- IPPROTO_CFTP
- IPPROTO_HELLO
- IPPROTO_SATEXPAK
- IPPROTO_KRYPTOLAN
- IPPROTO_RVD
- IPPROTO_IPPC
- IPPROTO_ADFS
- IPPROTO_SATMON
- IPPROTO_VISA
- IPPROTO_IPCV
- IPPROTO_CPNX
- IPPROTO_CPHB
- IPPROTO_WSN
- IPPROTO_PVP
- IPPROTO_BRSATMON
- IPPROTO_ND
- IPPROTO_WBMON
- IPPROTO_WBEXPAK
- IPPROTO_EON
- IPPROTO_VMTP
- IPPROTO_SVMTP
- IPPROTO_VINES
- IPPROTO_TTP
- IPPROTO_IGP
- IPPROTO_DGP
- IPPROTO_TCF
- IPPROTO_IGRP
- IPPROTO_OSPFIGP
- IPPROTO_SRPC
- IPPROTO_LARP
- IPPROTO_MTP
- IPPROTO_AX25
- IPPROTO_IPEIP
- IPPROTO_MICP
- IPPROTO_SCCSP
- IPPROTO_ETHERIP
- IPPROTO_ENCAP
- IPPROTO_APES
- IPPROTO_GMTP
- IPPROTO_PIM
- IPPROTO_IPCOMP
- IPPROTO_PGM
- IPPROTO_SCTP
- IPPROTO_DIVERT
- IPPROTO_RAW

**Use for `shutdown` and `close` API**

- SHUT_RD
- SHUT_WR
- SHUT_RDWR


**Use for `send`, `sendto`, `recv` and `recvfrom` API**

- MSG_*

## llsocket.inet API


### bind( host, port, socktype [, nonblock [, reuseaddr [, protocol]]] )

create bound socket.

**Parameters**

- host: host string.
- port: port number string.
- socktype: SOCK_*.
- nonblock: set nonblock flag to created socket if specified to true.
- reuseaddr: set reuseaddr flag to created socket if specified to true.
- protocol: IPROTO_*
 
**Returns**

1. fd: socket.
2. err: error number.


### connect( host, port, socktype[, nonblock [, reuseaddr [, protocol]]] )

create connected socket.

**Parameters** and **Returnes** is same as `bind( host, port, socktype, nonblock, reuseaddr )` API.


## llsocket.unix API

### bind( path, socktype [, nonblock [, protocol]] )

create bound unix domain socket.

**Parameters**

- path: string.
- socktype: SOCK_*.
- nonblock: set nonblock flag to created socket if specified to true.
- protocol: IPROTO_*

**Returns**

1. fd: socket.
2. err: error number.


### connect( path, socktype [, nonblock [, protocol]] )

create connected unix domain socket.

**Parameters** and **Returnes** is same as `bind( path, socktype, nonblock )` API.


### socketpair( socktype [, nonblock [, protocol] ] )

create a pair of connected sockets

**Parameters**

- socktype: SOCK_*.
- nonblock: set nonblock flag to created socket if specified to true.
- protocol: IPPROTO_*.

**Returns**

1. fd: socket[0].
2. fd: socket[1].
3. err: error number.


## llsocket API

### sockname( fd )

returns the current address for socket.

**Parameters**

- fd: socket.

**Returns**

1. address: address.
2. err: error sttring.


### peername( fd )

get address of connected peer.

**Parameters**

- fd: socket.

**Returns**

1. address: `llsocket.addr` userdata.
2. err: error string.


### atmark( fd )

determine whether socket is at out-of-band mark.

**Parameters**

- fd: socket.

**Returns**

1. boolean: a true on marked, or false on unmarked.
2. err: error string.


### type( fd )

returns the SO_TYPE value of socket option.

**Parameters**

- fd: socket.

**Returns**

1. type: socket type as a number value.
2. err: error string.


### error( fd )

returns the SO_ERROR value of socket option. **read only**

**Parameters**

- fd: socket.

**Returns**

1. flag: current option state as a number value.
2. err: error string.


### shutdown( fd, how )

shut down socket.

**Parameters**

- fd: socket
- how: SHUT_*

**Returns**

1. err: error string.


### close( fd [, how] )

close socket.

**Parameters**

- fd: socket.
- how: SHUT_*, call shutdown before closing if specified.

**Returns**

1. err: error string.


### listen( fd [, backlog] )

listen for connections on a socket.

**Parameters**

- fd: socket.
- backlog: maximum length for the queue of pending connections. default SOMAXCONN.

**Returns**

1. err: error string.


### accept( fd )

accept a connection on a socket.

**Parameters**

- fd: socket.

**Returns**

1. fd: connected socket.
2. err: error string.
3. again: true if errno is EAGAIN, EWOULDBLOCK, EINTR or ECONNABORTED.


### acceptInherits( fd )

accept a connection on a socket. this connection inherits a O_NONBLOCK flag from the listening socket. 

**NOTE: this behavior works only on linux.**

**Parameters**

- fd: socket.

**Returns**

1. fd: connected socket.
2. err: error string.
3. true if errno is EAGAIN, EWOULDBLOCK, EINTR or ECONNABORTED.


### recv( fd [, len [, flag, ...]] )

receive a message from a socket.

**Parameters**

- fd: socket.
- len: number of bytes to receive. default 4096 bytes.
- flag: recv flag.


**Returns**

1. str: received message on success, or nil if closed by peer or got an error.
2. err: an error string on failure, or a nil if a again is a true.
3. again: true if errno is EAGAIN, EWOULDBLOCK or EINTR.


### recvfrom( fd [, len [, flag, ...]] )

receive a message from a socket.

**Parameters**

same as `recv( fd [, len [, flag, ...]] )` API.

**Returns**

1. str: received message on success, or nil if closed by peer or got an error.
2. addr: `llsocket.addr` userdata or nil.
2. err: an error string on failure, or a nil if a again is a true.
4. again: true if errno is EAGAIN, EWOULDBLOCK or EINTR.


### send( fd, msg [, flag, ...] )

send a message from a socket.

**Parameters**

- fd: socket.
- str: message string.
- flag: send flag.


**Returns**

1. bytes: number of bytes sent on success, or nil if closed by peer.
2. err: an error string on failure, or a nil if a again is a true.
3. again: true if errno is EAGAIN, EWOULDBLOCK or EINTR.


### sendto( fd, msg [, addr, [flag, ...]] )

send a message from a socket.

**Parameters**

- fd: socket.
- str: message string.
- addr: `llsocket.addr` userdata or nil.
- flag: send flag.


**Returns**

same as `send( fd, msg [, flag, ...] )` API.


### sendfile( fd, filFd, len [, offset] )

send a file to a socket.

**Parameters**

- fd: socket.
- fileFd: file descriptor.
- len: how many bytes of the file should be sent.
- offset: where to begin in the file. default 0.


**Returns**

same as `send( fd, msg [, flag, ...] )` API.



## llsocket.opt API

the following API uses for getting or setting socket options.

### cloexec( fd [, flag] )

returns the current FD_CLOEXEC flag for socket.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current flag state as a boolean value.
2. err: error string.

### nonblock( fd [, flag] )

returns the current O_NONBLOCK flag for socket.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current flag state as a boolean value.
2. err: error string.


### tcpnodelay( fd [, flag] )

returns the TCP_NODELAY value of socket option.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current option state as a boolean value.
2. err: error string.


### tcpcork( fd [, flag] )

returns the TCP_CORK or TCP_NOPUSH value of socket option.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current option state as a boolean value.
2. err: error string.


### reuseaddr( fd [, flag] )

returns the SO_REUSEADDR value of socket option.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current option state as a boolean value.
2. err: error string.

### broadcast( fd [, flag] )

returns the SO_BROADCAST value of socket option.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current option state as a boolean value.
2. err: error string.


### debug( fd [, flag] )

returns the SO_DEBUG value of socket option.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current option state as a boolean value.
2. err: error string.


### keepalive( fd [, flag] )

returns the SO_KEEPALIVE value of socket option.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current option state as a boolean value.
2. err: error string.


### oobinline( fd [, flag] )

returns the SO_OOBINLINE value of socket option.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current option state as a boolean value.
2. err: error string.


### timestamp( fd [, flag] )

returns the SO_TIMESTAMP value of socket option.

**Parameters**

- fd: socket.
- flag: to enabled on true, or to disabled on false.

**Returns**

1. flag: current option state as a boolean value.
2. err: error string.


### rcvbuf( fd [, size] )

returns the SO_RCVBUF value of socket option.

**Parameters**

- fd: socket.
- size: set size of receive buffer for socket if specified.

**Returns**

1. size: current option state as a number value.
2. err: error string.

### rcvlowat( fd [, size] )

returns the SO_RCVLOWAT value of socket option.

**Parameters**

- fd: socket.
- size: set minimum size of receive buffer for socket if specified.

**Returns**

1. size: current option state as a number value.
2. err: error string.


### sndbuf( fd [, size] )

returns the SO_SNDBUF value of socket option.

**Parameters**

- fd: socket.
- size: set size of send buffer for socket if specified.

**Returns**

1. size: current option state as a number value.
2. err: error string.

### sndlowat( fd [, size] )

returns the SO_SNDLOWAT value of socket option.

**Parameters**

- fd: socket.
- size: set minimum size of send buffer for socket if specified.

**Returns**

1. size: current option state as a number value.
2. err: error string.



### rcvtimeo( fd [, sec] )

returns the SO_RCVTIMEO value of socket option.

**Parameters**

- fd: socket.
- sec: set value of receive-timeout for socket if specified.

**Returns**

1. sec: current option state as a number value.
2. err: error string.

### sndtimeo( fd [, sec] )

returns the SO_SNDTIMEO value of socket option.

**Parameters**

- fd: socket.
- sec: set value of send-timeout for socket if specified.

**Returns**

1. sec: current option state as a number value.
2. err: error string.



## TODO

- sendmsg
- recvmsg
- raw socket

