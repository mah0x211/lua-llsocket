# lua-llsocket

low-level socket module.

**NOTE: Do not use this module.  this module is under heavy development.**


## Installation

```bash
$ luarocks install llsocket --from=http://mah0x211.github.io/rocks/
```


## Constants

these constants defined at the `llsocket.*`

### AF_* Types

**TODO**


### AI_* Flags

- AI_PASSIVE
- AI_CANONNAME
- AI_NUMERICHOST
- AI_NUMERICSERV
- AI_ALL
- AI_V4MAPPED_CFG
- AI_ADDRCONFIG
- AI_V4MAPPED
- AI_DEFAULT


### IPPROTO_* Types

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


### MSG_* FLAGS

- MSG_CMSG_CLOEXEC
- MSG_CONFIRM
- MSG_CTRUNC
- MSG_DONTROUTE
- MSG_DONTWAIT
- MSG_EOR
- MSG_ERRQUEUE
- MSG_MORE
- MSG_NOSIGNAL
- MSG_OOB
- MSG_PEEK
- MSG_TRUNC
- MSG_WAITALL


### NI_* Flags

- NI_NOFQDN
- NI_NUMERICHOST
- NI_NAMEREQD
- NI_NUMERICSERV
- NI_DGRAM
- NI_WITHSCOPEID


### SOCK_* Types

- SOCK_STREAM
- SOCK_DGRAM
- SOCK_RAW
- SOCK_RDM
- SOCK_SEQPACKET


### SHUT_* Flags

- SHUT_RD
- SHUT_WR
- SHUT_RDWR


## llsocket.inet Module

`llsocket.inet` module has the following functions.


### arr, err = inet.getaddrinfo( host [, port [, socktype [, protocol [, flag, ...]]]] )

get a list of address info of tcp stream socket.

- **Parameters**
	- `host:string`: host string.
	- `port:string`: either a decimal port number or a service name listed in services(5).
	- `socktype:number` [SOCK_* types](#sock-types) constants.
	- `protocol:number`: [IPROTO_* types](#ipproto-types) constants.
	- `flags:...`: [AI_* flags](#ai-flags) constants.
- **Returns**
	- `arr:table`: list of [addrinfo](#llsocketaddrinfo-instance-methods).
	- `err:string`: error string.


### arr, err = inet.getaddrinfo6( host [, port [, socktype [, protocol [, flag, ...]]]] )

get a list of IPv6 address info of tcp stream socket.

- **Parameters**
	- `host:string`: host string.
	- `port:string`: either a decimal port number or a service name listed in services(5).
	- `socktype:number` [SOCK_* types](#sock-types) constants.
	- `protocol:number`: [IPROTO_* types](#ipproto-types) constants.
	- `flags:...`: [AI_* flags](#ai-flags) constants.
- **Returns**
	- `arr:table`: list of [addrinfo](#llsocketaddrinfo-instance-methods).
	- `err:string`: error string.


## llsocket.unix Module

`llsocket.unix` module has the following functions.


### arr, err = unix.getaddrinfo( path [, socktype [, protocol [, flag, ...]]] )

get a list of AF_UNIX address info.

- **Parameters**
	- `host:string`: host string.
	- `port:string`: either a decimal port number or a service name listed in services(5).
	- `socktype:number` [SOCK_* types](#sock-types) constants.
	- `protocol:number`: [IPROTO_* types](#ipproto-types) constants.
	- `flags:...`: [AI_* flags](#ai-flags) constants.
- **Returns**
	- `arr:table`: list of [addrinfo](#llsocketaddrinfo-instance-methods).
	- `err:string`: error string.


## llsocket.addrinfo Instance Methods

`llsocket.addrinfo` (address info) instance has following methods.

### ni, err = ai:nameinfo( [flag, ...] )

get hostname and service name.

- **Parameters**
	- `flag:...`: [AI_* flags](#ai-flags) constants.
- **Returns**
	- `ni:table`: name info table.
		- `host:string`: hostname.
		- `serv:string`: service name.
	- `err:string`: error string.


## llsocket.socket Module

`llsocket.socket` module has the following functions.


### socks, err = socket.pair( socktype [, nonblock [, protocol]] )

create a pair of connected sockets.

- **Parameters**
	- `socktype:number` [SOCK_* types](#sock-types) constants.
	- `nonblock:boolean`: enable the O_NONBLOCK flag.
	- `protocol:number`: [IPROTO_* types](#ipproto-types) constants.
- **Returns**
	- `socks:table`: pair of connected sockets.
		- `1:socket`: instance of [socket](#llsocketsocket-instance-methods)
		- `2:socket`: instance of [socket](#llsocketsocket-instance-methods)
	- `err:string`: error string.


### sock, err = socket.new( ai, nonblock )

create an instance of socket

- **Parameters**
	- `ai:addrinfo`: instance of [addrinfo](#llsocketaddrinfo-instance-methods).
	- `nonblock:boolean`: enable the O_NONBLOCK flag.
- **Returns**
	- `sock:socket`: instance of [socket](#llsocketsocket-instance-methods).
	- `err:string`: error string.



## llsocket.socket Instance Methods


### sock, err = sock:dup( [ai] )

duplicate an instance of socket.

- **Parameters**
	- `ai:addrinfo`: instance of [addrinfo](#llsocketaddrinfo-instance-methods).
- **Returns**
	- `sock:socket`: instance of [socket](#llsocketsocket-instance-methods).
	- `err:string`: error string.


### fd = sock:fd()

get a socket file descriptor.

- **Returns**
	- `fd:number`: socket file descriptor.


### af = sock:family()

get a address family type.

- **Returns**
	- `af:number`: [AF_* types](#af-types) constants.


### st = sock:socktype()

get a socket type.

- **Returns**
	- `st:number`: [SOCK_* type](#sock-types) constants.


### proto = sock:protocol()

get a protocol type.

- **Returns**
	- `pt:number`: [IPPROTO_* type](#ipproto-types) constants.


### err = sock:bind( [ai] )

bind the socket to an address.

- **Parameters**
	- `ai:addrinfo`: instance of [addrinfo](#llsocketaddrinfo-instance-methods).
- **Returns**
	- `err:string`: error string.


### err = sock:connect( [ai] )

initiate a new connection.

- **Parameters**
	- `ai:addrinfo`: instance of [addrinfo](#llsocketaddrinfo-instance-methods).
- **Returns**
	- `err:string`: error string.


### err = sock:shutdown( [flag] )

shut down part of a full-duplex connection.

- **Parameters**
	- `flag:number`: [SHUT_* flag](#shut-flags) constants.
- **Returns**
	- `err:string`: error string.


### err = sock:close( [flag] )

close a file descriptor.

- **Parameters**
	- `flag:number`: [SHUT_* flag](#shut-flags) constants.
- **Returns**
	- `err:string`: error string.


### err = sock:listen( [backlog] )

listen for connections.

- **Parameters**
	- `backlog:number`: the maximum length for the queue of pending connections.
- **Returns**
	- `err:string`: error string.


### sock, err, again = sock:accept()

accept a connection.

- **Returns**
	- `sock:socket`: instance of [socket](#llsocketsocket-instance-methods).
	- `err:string`: error string.
	- `again:bool`: true if errno is EAGAIN, EWOULDBLOCK, EINTR or ECONNABORTED.


### len, err, again = sock:send( msg [, flag, ...] )

send a message from a socket.

- **Parameters**
	- `msg:string`: message string.
	- `flag:...`: [MSG_* flags](#msg-flags) constants.
- **Returns**
		- `len:number`: the number of bytes sent.
		- `err:string`: error string.
		- `again:bool`: true if len != #str, or errno is EAGAIN, EWOULDBLOCK or EINTR.

**NOTE:** all return values will be nil if closed by peer.


### len, err, again = sock:sendto( msg, ai [, flag, ...] )

send a message to specified destination address.

- **Parameters**
	- `msg:string`: message string.
	- `addr:addrinfo`: instance of [addrinfo](#llsocketaddrinfo-instance-methods).
	- `flag:...`: [MSG_* flags](#msg-flags) constants.
- **Returns**
	- `len:number`: the number of bytes sent.
	- `err:string`: error string.
	- `again:bool`: true if len != #str, or errno is EAGAIN, EWOULDBLOCK or EINTR.

**NOTE:** all return values will be nil if closed by peer.


### len, err, again = sock:sendfile( fd, bytes [, offset] )

send a file.

- **Parameters**
	- `fd:number`: file descriptor.
	- `bytes:number`: how many bytes of the file should be sent.
	- `offset:number`: where to begin in the file.
- **Returns**
	- `len:number`: the number of bytes sent.
	- `err:string`: error string.
	- `again:bool`: true if len != #str, or errno is EAGAIN or EINTR.

**NOTE:** all return values will be nil if closed by peer.


### msg, err, again = sock:recv( [bufsize [, flag, ...]] )

receive a message.

- **Parameters**
	- `bufsize:number`: working buffer size of receive operation.
	- `flag:...`: [MSG_* flags](#msg-flags) constants.
- **Returns**
	- `msg:string`: received message string.
	- `err:string`: error string.
	- `again:bool`: true if errno is EAGAIN, EWOULDBLOCK or EINTR.

**NOTE:** all return values will be nil if closed by peer.


### msg, ai, err, again = sock:recvfrom( [bufsize [, flag, ...]] )

receive message and address info.

- **Parameters**
	- `bufsize:number`: working buffer size of receive operation.
	- `flag:...`: [MSG_* flags](#msg-flags) constants.
- **Returns**
	- `msg:string`: received message string.
	- `ai:addrinfo`: instance of [addrinfo](#llsocketaddrinfo-instance-methods).
	- `err:string`: error string.
	- `again:bool`: true if errno is EAGAIN, EWOULDBLOCK or EINTR.

**NOTE:** all return values will be nil if closed by peer.

***

# TODO

## Documentation

- bool, err = sock:atmark()
- ai, err = sock:getsockname()
- ai, err = sock:getpeername()
- enable, err = sock:cloexec( [enable] )
- enable, err = sock:nonblock( [enable] )
- errno, err = sock:error()
- bool, err = sock:acceptconn()
- enable, err = sock:tcpnodelay( [enable] )
- sec, err = sock:tcpkeepintvl( [sec] )
- cnt, err = sock:tcpkeepcnt( [cnt] )
- sec, err = sock:tcpkeepalive( [sec] )
- enable, err = sock:tcpcork( [enable] )
- enable, err = sock:reuseport( [enable] )
- enable, err = sock:reuseaddr( [enable] )
- enable, err = sock:broadcast( [enable] )
- enable, err = sock:debug( [enable] )
- enable, err = sock:keepalive( [enable] )
- enable, err = sock:oobinline( [enable] )
- enable, err = sock:dontroute( [enable] )
- enable, err = sock:timestamp( [enable] )
- sz, err = sock:rcvbuf( [sz] )
- sz, err = sock:rcvlowat( [sz] )
- sz, err = sock:sndbuf( [sz] )
- sz, err = sock:sndlowat( [sz] )
- sec, err = sock:rcvtimeo( [sec] )
- sec, err = sock:sndtimeo( [sec] )
- enable, err = sock:mcastloop( [enable] )
- ttl, err = sock:mcastttl( [ttl] )
- ifname, err = sock:mcastif( [ifname] )
- err = sock:mcastjoin( mcaddr [, ifname] )
- err = sock:mcastleave( mcaddr [, ifname] )
- err = sock:mcastjoinsrc( mcaddr, srcaddr [, ifname] )
- err = sock:mcastleavesrc( mcaddr, srcaddr [, ifname] )
- err = sock:mcastblocksrc( mcaddr, srcaddr [, ifname] )
- err = sock:mcastunblocksrc( mcaddr, srcaddr [, ifname] )


## Implementation

- sendmsg
- recvmsg
