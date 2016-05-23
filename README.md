# lua-llsocket

low-level socket module.

**NOTE: Do not use this module.  this module is under heavy development.**


## Installation

```bash
$ luarocks install llsocket --from=http://mah0x211.github.io/rocks/
```


## llsocket.inet Module

`llsocket.inet` module has the following functions.


### arr, err = inet.getaddrinfo( host [, port [, socktype [, protocol [, flag, ...]]]] )

get a list of address info of tcp stream socket.

- **Parameters**
    - `host:string`: host string.
    - `port:string`: either a decimal port number or a service name listed in services(5).
    - `socktype:number` [SOCK_* types](#sock_-types) constants.
    - `protocol:number`: [IPROTO_* types](#ipproto_-types) constants.
    - `flags:...`: [AI_* flags](#ai_-flags) constants.
- **Returns**
    - `arr:table`: list of [addrinfo](#llsocketaddrinfo-instance-methods).
    - `err:string`: error string.


### arr, err = inet.getaddrinfo6( host [, port [, socktype [, protocol [, flag, ...]]]] )

get a list of IPv6 address info of tcp stream socket.

- **Parameters**
    - `host:string`: host string.
    - `port:string`: either a decimal port number or a service name listed in services(5).
    - `socktype:number` [SOCK_* types](#sock_-types) constants.
    - `protocol:number`: [IPROTO_* types](#ipproto_-types) constants.
    - `flags:...`: [AI_* flags](#ai_-flags) constants.
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
    - `socktype:number` [SOCK_* types](#sock_-types) constants.
    - `protocol:number`: [IPROTO_* types](#ipproto_-types) constants.
    - `flags:...`: [AI_* flags](#ai-flags) constants.
- **Returns**
    - `arr:table`: list of [addrinfo](#llsocketaddrinfo-instance-methods).
    - `err:string`: error string.


## llsocket.addrinfo Instance Methods

`llsocket.addrinfo` (address info) instance has following methods.

### ni, err = ai:nameinfo( [flag, ...] )

get hostname and service name.

- **Parameters**
    - `flag:...`: [AI_* flags](#ai_-flags) constants.
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
    - `socktype:number` [SOCK_* types](#sock_-types) constants.
    - `nonblock:boolean`: enable the O_NONBLOCK flag.
    - `protocol:number`: [IPROTO_* types](#ipproto_-types) constants.
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
    - `af:number`: [AF_* types](#af_-types) constants.


### st = sock:socktype()

get a socket type.

- **Returns**
    - `st:number`: [SOCK_* type](#sock_-types) constants.


### proto = sock:protocol()

get a protocol type.

- **Returns**
    - `pt:number`: [IPPROTO_* type](#ipproto_-types) constants.


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
    - `flag:number`: [SHUT_* flag](#shut_-flags) constants.
- **Returns**
    - `err:string`: error string.


### err = sock:close( [flag] )

close a file descriptor.

- **Parameters**
    - `flag:number`: [SHUT_* flag](#shut_-flags) constants.
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
    - `flag:...`: [MSG_* flags](#msg_-flags) constants.
- **Returns**
    - `len:number`: the number of bytes sent.
    - `err:string`: error string.
    - `again:bool`: true if len != #msg, or errno is EAGAIN, EWOULDBLOCK or EINTR.

**NOTE:** all return values will be nil if closed by peer.


### len, err, again = sock:sendto( msg, ai [, flag, ...] )

send a message to specified destination address.

- **Parameters**
    - `msg:string`: message string.
    - `addr:addrinfo`: instance of [addrinfo](#llsocketaddrinfo-instance-methods).
    - `flag:...`: [MSG_* flags](#msg_-flags) constants.
- **Returns**
    - `len:number`: the number of bytes sent.
    - `err:string`: error string.
    - `again:bool`: true if len != #msg, or errno is EAGAIN, EWOULDBLOCK or EINTR.

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
    - `again:bool`: true if len != #bytes, or errno is EAGAIN or EINTR.

**NOTE:** all return values will be nil if closed by peer.


### msg, err, again = sock:recv( [bufsize [, flag, ...]] )

receive a message.

- **Parameters**
    - `bufsize:number`: working buffer size of receive operation.
    - `flag:...`: [MSG_* flags](#msg_-flags) constants.
- **Returns**
    - `msg:string`: received message string.
    - `err:string`: error string.
    - `again:bool`: true if errno is EAGAIN, EWOULDBLOCK or EINTR.

**NOTE:** all return values will be nil if closed by peer.


### msg, ai, err, again = sock:recvfrom( [bufsize [, flag, ...]] )

receive message and address info.

- **Parameters**
    - `bufsize:number`: working buffer size of receive operation.
    - `flag:...`: [MSG_* flags](#msg_-flags) constants.
- **Returns**
    - `msg:string`: received message string.
    - `ai:addrinfo`: instance of [addrinfo](#llsocketaddrinfo-instance-methods).
    - `err:string`: error string.
    - `again:bool`: true if errno is EAGAIN, EWOULDBLOCK or EINTR.

**NOTE:** all return values will be nil if closed by peer.


### bool, err = sock:atmark()

determine whether socket is at out-of-band mark.

- **Returns**
    - `bool:boolean`: true if the socket is at the out-of-band mark.
    - `err:string`: error string.


### ai, err = sock:getsockname()

get socket name.

- **Returns**
    - `ai:addrinfo`: instance of [addrinfo](#llsocketaddrinfo-instance-methods).
    - `err:string`: error string.


### ai, err = sock:getpeername()

get address of connected peer.

- **Returns**: same as [sock:getsockname](#ai-err--sockgetsockname).


### enable, err = sock:cloexec( [enable] )

determine whether the FD_CLOEXEC flag enabled, or change the state to an argument value.

- **Parameters**
    - `enable:boolean`: to enable or disable the FD_CLOEXEC flag.
- **Returns**
    - `enable:boolean`: state of the FD_CLOEXEC flag.
    - `err:string`: error string.


### enable, err = sock:nonblock( [enable] )

determine whether the O_NONBLOCK flag enabled, or change the state to an argument value.

- **Parameters**
    - `enable:boolean`: to enable or disable the O_NONBLOCK flag.
- **Returns**
    - `enable:boolean`: state of the O_NONBLOCK flag.
    - `err:string`: error string.


### errno, err = sock:error()

get pending socket error status with and clears it.

- **Returns**
    - `errno:number`: number of last error.
    - `err:string`: error string.


### bool, err = sock:acceptconn()

determine whether the SO_ACCEPTCONN flag enabled.

- **Returns**
    - `bool:boolean`: state of the SO_ACCEPTCONN flag.
    - `err:string`: error string.


### enable, err = sock:tcpnodelay( [enable] )

determine whether the TCP_NODELAY flag enabled, or change the state to an argument value.

- **Parameters**
    - `enable:boolean`: to enable or disable the TCP_NODELAY flag.
- **Returns**
    - `enable:boolean`: state of the TCP_NODELAY flag.
    - `err:string`: error string.


### sec, err = sock:tcpkeepintvl( [sec] )

get the TCP_KEEPINTVL value, or change that value to an argument value.

- **Parameters**
    - `sec:number`: set the TCP_KEEPINTVL value.
- **Returns**
    - `sec:number`: value of the TCP_KEEPINTVL.
    - `err:string`: error string.


### cnt, err = sock:tcpkeepcnt( [cnt] )

get the TCP_KEEPCNT value, or change that value to an argument value.

- **Parameters**
    - `cnt:number`: set the TCP_KEEPCNT value.
- **Returns**
    - `sec:number`: value of the TCP_KEEPCNT.
    - `err:string`: error string.


### sec, err = sock:tcpkeepalive( [sec] )

get the TCP_KEEPALIVE value, or set that value if argument passed.

- **Parameters**
    - `sec:number`: set the TCP_KEEPALIVE value.
- **Returns**
    - `sec:number`: value of the TCP_KEEPALIVE.
    - `err:string`: error string.


### enable, err = sock:tcpcork( [enable] )

determine whether the TCP_CORK flag enabled, or change the state to an argument value.

- **Parameters**
    - `enable:boolean`: to enable or disable the TCP_CORK flag.
- **Returns**
    - `enable:boolean`: state of the TCP_CORK flag.
    - `err:string`: error string.


### enable, err = sock:reuseport( [enable] )

determine whether the SO_REUSEPORT flag enabled, or change the state to an argument value.

- **Parameters**
    - `enable:boolean`: to enable or disable the SO_REUSEPORT flag.
- **Returns**
    - `bool:boolean`: state of the SO_REUSEADDR flag.
    - `err:string`: error string.


### enable, err = sock:reuseaddr( [enable] )

determine whether the SO_REUSEADDR flag enabled, or change the state to an argument value.

- **Parameters**
    - `enable:boolean`: to enable or disable the SO_REUSEADDR flag.
- **Returns**
    - `enable:boolean`: state of the SO_REUSEADDR flag.
    - `err:string`: error string.


### enable, err = sock:broadcast( [enable] )

determine whether the SO_BROADCAST flag enabled, or change the state to an argument value.

- **Parameters**
    - `enable:boolean`: to enable or disable the SO_BROADCAST flag.
- **Returns**
    - `enable:boolean`: state of the SO_BROADCAST flag.
    - `err:string`: error string.


### enable, err = sock:debug( [enable] )

determine whether the SO_DEBUG flag enabled, or change the state to an argument value.

- **Parameters**
    - `enable:boolean`: to enable or disable the SO_DEBUG flag.
- **Returns**
    - `enable:boolean`: state of the SO_DEBUG flag.
    - `err:string`: error string.


### enable, err = sock:keepalive( [enable] )

determine whether the SO_KEEPALIVE flag enabled, or change the state to an argument value.

- **Parameters**
    - `enable:boolean`: to enable or disable the SO_KEEPALIVE flag.
- **Returns**
    - `enable:boolean`: state of the SO_KEEPALIVE flag.
    - `err:string`: error string.


### enable, err = sock:oobinline( [enable] )

determine whether the SO_OOBINLINE flag enabled, or change the state to an argument value.

- **Parameters**
    - `enable:boolean`: to enable or disable the SO_OOBINLINE flag.
- **Returns**
    - `enable:boolean`: state of the SO_OOBINLINE flag.
    - `err:string`: error string.


### enable, err = sock:dontroute( [enable] )

determine whether the SO_DONTROUTE flag enabled, or change the state to an argument value.

- **Parameters**
    - `enable:boolean`: to enable or disable the SO_DONTROUTE flag.
- **Returns**
    - `enable:boolean`: state of the SO_DONTROUTE flag.
    - `err:string`: error string.


### enable, err = sock:timestamp( [enable] )

determine whether the SO_TIMESTAMP flag enabled, or change the state to an argument value.

- **Parameters**
    - `enable:boolean`: to enable or disable the SO_TIMESTAMP flag.
- **Returns**
    - `enable:boolean`: state of the SO_TIMESTAMP flag.
    - `err:string`: error string.


### sz, err = sock:rcvbuf( [sz] )

get the SO_RCVBUF value, or change that value to an argument value.

- **Parameters**
    - `sz:number`: set the SO_RCVBUF value.
- **Returns**
    - `sz:number`: value of the SO_RCVBUF.
    - `err:string`: error string.


### sz, err = sock:rcvlowat( [sz] )

get the SO_RCVLOWAT value, or change that value to an argument value.

- **Parameters**
    - `sz:number`: set the SO_RCVLOWAT value.
- **Returns**
    - `sz:number`: value of the SO_RCVLOWAT.
    - `err:string`: error string.


### sz, err = sock:sndbuf( [sz] )

get the SO_SNDBUF value, or change that value to an argument value.

- **Parameters**
    - `sz:number`: set the SO_SNDBUF value.
- **Returns**
    - `sz:number`: value of the SO_SNDBUF.
    - `err:string`: error string.


### sz, err = sock:sndlowat( [sz] )

get the SO_SNDLOWAT value, or change that value to an argument value.

- **Parameters**
    - `sz:number`: set the SO_SNDLOWAT value.
- **Returns**
    - `sz:number`: value of the SO_SNDLOWAT.
    - `err:string`: error string.


### sec, err = sock:rcvtimeo( [sec] )

get the SO_RCVTIMEO value, or change that value to an argument value.

- **Parameters**
    - `sec:number`: set the SO_RCVTIMEO value.
- **Returns**
    - `sec:number`: value of the SO_RCVTIMEO.
    - `err:string`: error string.


### sec, err = sock:sndtimeo( [sec] )

get the SO_SNDTIMEO value, or change that value to an argument value.

- **Parameters**
    - `sec:number`: set the SO_SNDTIMEO value.
- **Returns**
    - `sec:number`: value of the SO_SNDTIMEO.
    - `err:string`: error string.


### enable, err = sock:mcastloop( [enable] )

determine whether the IP_MULTICAST_LOOP flag enabled, or change the state to an argument value.

- **Parameters**
    - `enable:boolean`: to enable or disable the IP_MULTICAST_LOOP flag.
- **Returns**
    - `enable:boolean`: state of the IP_MULTICAST_LOOP flag.
    - `err:string`: error string.


### ttl, err = sock:mcastttl( [ttl] )

get the IP_MULTICAST_TTL value, or change that value to an argument value.

- **Parameters**
    - `ttl:number`: set the IP_MULTICAST_TTL value.
- **Returns**
    - `sec:number`: value of the IP_MULTICAST_TTL.
    - `err:string`: error string.


### ifname, err = sock:mcastif( [ifname] )

get the IP_MULTICAST_IF value, or change that value to an argument value.

- **Parameters**
    - `ifname:string`: set the IP_MULTICAST_IF value.
- **Returns**
    - `ifnames:string`: value of the IP_MULTICAST_IF.
    - `err:string`: error string.


### err = sock:mcastjoin( mcaddr [, ifname] )

set the IP_ADD_MEMBERSHIP or IPV6_JOIN_GROUP (if IPv6) value.

- **Parameters**
    - `mcaddr:string`: multicast group address.
    - `ifname:string`: interface name.
- **Returns**
    - `err:string`: error string.


### err = sock:mcastleave( mcaddr [, ifname] )

set the IP_DROP_MEMBERSHIP or IPV6_LEAVE_GROUP (if IPv6) value.

- **Parameters**
    - `mcaddr:string`: multicast group address.
    - `ifname:string`: interface name.
- **Returns**
    - `err:string`: error string.


### err = sock:mcastjoinsrc( mcaddr, srcaddr [, ifname] )

set the IP_ADD_SOURCE_MEMBERSHIP or MCAST_JOIN_SOURCE_GROUP (if IPv6) value.

- **Parameters**
    - `mcaddr:string`: multicast group address.
    - `srcaddr:string`: multicast source address.
    - `ifname:string`: interface name.
- **Returns**
    - `err:string`: error string.


### err = sock:mcastleavesrc( mcaddr, srcaddr [, ifname] )

set the IP_DROP_SOURCE_MEMBERSHIP or MCAST_LEAVE_SOURCE_GROUP (if IPv6) value.

- **Parameters**
    - `mcaddr:string`: multicast group address.
    - `srcaddr:string`: multicast source address.
    - `ifname:string`: interface name.
- **Returns**
    - `err:string`: error string.


### err = sock:mcastblocksrc( mcaddr, srcaddr [, ifname] )

set the IP_BLOCK_SOURCE or MCAST_BLOCK_SOURCE (if IPv6) value.

- **Parameters**
    - `mcaddr:string`: multicast group address.
    - `srcaddr:string`: multicast source address.
    - `ifname:string`: interface name.
- **Returns**
    - `err:string`: error string.


### err = sock:mcastunblocksrc( mcaddr, srcaddr [, ifname] )

set the IP_UNBLOCK_SOURCE or MCAST_UNBLOCK_SOURCE (if IPv6) value.

- **Parameters**
    - `mcaddr:string`: multicast group address.
    - `srcaddr:string`: multicast source address.
    - `ifname:string`: interface name.
- **Returns**
    - `err:string`: error string.



## Constants

these constants defined at the `llsocket.*`

### AF_* Types

- `AF_APPLETALK`: Apple Talk
- `AF_CCITT`: CCITT protocols, X.25 etc
- `AF_CHAOS`: mit CHAOS protocols
- `AF_CNT`: Computer Network Technology
- `AF_COIP`: connection-oriented IP, aka ST II
- `AF_DATAKIT`: datakit protocols
- `AF_DECnet`: DECnet
- `AF_DLI`: DEC Direct data link interface
- `AF_E164`: CCITT E.164 recommendation
- `AF_ECMA`: European computer manufacturers
- `AF_HYLINK`: NSC Hyperchannel
- `AF_IEEE80211`: IEEE 802.11 protocol
- `AF_IMPLINK`: arpanet imp addresses
- `AF_INET6`: IPv6
- `AF_INET`: internetwork: UDP, TCP, etc.
- `AF_IPX`: Novell Internet Protocol
- `AF_ISDN`: Integrated Services Digital Network
- `AF_ISO`: ISO protocols
- `AF_LAT`: LAT
- `AF_LINK`: Link layer interface
- `AF_LOCAL`: backward compatibility
- `AF_NATM`: native ATM access
- `AF_NDRV`: Network Driver 'raw' access
- `AF_NETBIOS`: NetBIOS
- `AF_NS`: XEROX NS protocols
- `AF_OSI`: `AF_ISO`
- `AF_PPP`: PPP communication protocol
- `AF_PUP`: pup protocols: e.g. BSP
- `AF_RESERVED_36`: Reserved for internal usage
- `AF_ROUTE`: Internal Routing Protocol
- `AF_SIP`: Simple Internet Protocol
- `AF_SNA`: IBM SNA
- `AF_SYSTEM`: Kernel event messages
- `AF_UNIX`: local to host
- `AF_UNSPEC`: unspecified
- `AF_UTUN`


### AI_* Flags

- `AI_ADDRCONFIG`: only if any address is assigned
- `AI_ALL`: IPv6 and IPv4-mapped (with AI_V4MAPPED)
- `AI_CANONNAME`: fill ai_canonname
- `AI_DEFAULT`: (AI_V4MAPPED_CFG | AI_ADDRCONFIG)
- `AI_NUMERICHOST`: prevent host name resolution
- `AI_NUMERICSERV`: prevent service name resolution
- `AI_PASSIVE`: get address to use bind()
- `AI_V4MAPPED`: accept IPv4-mapped IPv6 address
- `AI_V4MAPPED_CFG`: accept IPv4-mapped if kernel supports


### IPPROTO_* Types

- `IPPROTO_3PC`: Third Party Connect
- `IPPROTO_ADFS`: Any distributed FS
- `IPPROTO_AHIP`: any host internal protocol
- `IPPROTO_AH`: IP6 Auth Header
- `IPPROTO_APES`: any private encr. scheme
- `IPPROTO_ARGUS`: Argus
- `IPPROTO_AX25`: AX.25 Frames
- `IPPROTO_BHA`: BHA
- `IPPROTO_BLT`: Bulk Data Transfer
- `IPPROTO_BRSATMON`: BackRoom SATNET Monitoring
- `IPPROTO_CFTP`: CFTP
- `IPPROTO_CHAOS`: Chaos
- `IPPROTO_CMTP`: Control Message Transport
- `IPPROTO_CPHB`: Comp. Prot. HeartBeat
- `IPPROTO_CPNX`: Comp. Prot. Net. Executive
- `IPPROTO_DDP`: Datagram Delivery
- `IPPROTO_DGP`: dissimilar gateway prot.
- `IPPROTO_DIVERT`: divert pseudo-protocol
- `IPPROTO_DONE`
- `IPPROTO_DSTOPTS`: IP6 destination option
- `IPPROTO_EGP`: exterior gateway protocol
- `IPPROTO_EMCON`: EMCON
- `IPPROTO_ENCAP`: encapsulation header
- `IPPROTO_EON`: ISO cnlp
- `IPPROTO_ESP`: IP6 Encap Sec. Payload
- `IPPROTO_ETHERIP`: Ethernet IP encapsulation
- `IPPROTO_FRAGMENT`: IP6 fragmentation header
- `IPPROTO_GGP`: gateway^2
- `IPPROTO_GMTP`: GMTP
- `IPPROTO_GRE`: General Routing Encap.
- `IPPROTO_HELLO`: "hello" routing protocol
- `IPPROTO_HMP`: Host Monitoring
- `IPPROTO_HOPOPTS`: IP6 hop-by-hop options
- `IPPROTO_ICMPV6`: ICMP6
- `IPPROTO_ICMP`: control message protocol
- `IPPROTO_IDPR`: InterDomain Policy Routing
- `IPPROTO_IDP`: xns idp
- `IPPROTO_IDRP`: InterDomain Routing
- `IPPROTO_IGMP`: group mgmt protocol
- `IPPROTO_IGP`: NSFNET-IGP
- `IPPROTO_IGRP`: Cisco
- `IPPROTO_IL`: IL transport protocol
- `IPPROTO_INLSP`: Integ. Net Layer Security
- `IPPROTO_INP`: Merit Internodal
- `IPPROTO_IPCOMP`: payload compression
- `IPPROTO_IPCV`: Packet Core Utility
- `IPPROTO_IPEIP`: IP encapsulated in IP
- `IPPROTO_IPPC`: Pluribus Packet Core
- `IPPROTO_IPV4`: IPv4 encapsulation
- `IPPROTO_IPV6`: IP6 header
- `IPPROTO_IP`: dummy for IP
- `IPPROTO_IRTP`: Reliable Transaction
- `IPPROTO_KRYPTOLAN`: Kryptolan
- `IPPROTO_LARP`: Locus Address Resoloution
- `IPPROTO_LEAF1`: Leaf-1
- `IPPROTO_LEAF2`: Leaf-2
- `IPPROTO_MAX`
- `IPPROTO_MEAS`: DCN Measurement Subsystems
- `IPPROTO_MHRP`: Mobile Host Routing
- `IPPROTO_MICP`: Mobile Int.ing control
- `IPPROTO_MTP`: Multicast Transport
- `IPPROTO_MUX`: Multiplexing
- `IPPROTO_ND`: Sun net disk proto
- `IPPROTO_NHRP`: Next Hop Resolution
- `IPPROTO_NONE`: IP6 no next header
- `IPPROTO_NSP`: Network Services
- `IPPROTO_NVPII`: network voice protocol
- `IPPROTO_OSPFIGP`: OSPFIGP
- `IPPROTO_PGM`: PGM
- `IPPROTO_PIGP`: private interior gateway
- `IPPROTO_PIM`: Protocol Independent Mcast
- `IPPROTO_PRM`: Packet Radio Measurement
- `IPPROTO_PUP`: pup
- `IPPROTO_PVP`: Packet Video Protocol
- `IPPROTO_RAW`: raw IP packet
- `IPPROTO_RCCMON`: BBN RCC Monitoring
- `IPPROTO_RDP`: Reliable Data
- `IPPROTO_ROUTING`: IP6 routing header
- `IPPROTO_RSVP`: resource reservation
- `IPPROTO_RVD`: Remote Virtual Disk
- `IPPROTO_SATEXPAK`: SATNET
- `IPPROTO_SATMON`: Satnet Monitoring
- `IPPROTO_SCCSP`: Semaphore Comm. security
- `IPPROTO_SCTP`: SCTP
- `IPPROTO_SDRP`: Source Demand Routing
- `IPPROTO_SEP`: Sequential Exchange
- `IPPROTO_SRPC`: Strite RPC protocol
- `IPPROTO_ST`: Stream protocol II
- `IPPROTO_SVMTP`: Secure VMTP
- `IPPROTO_SWIPE`: IP with encryption
- `IPPROTO_TCF`: TCF
- `IPPROTO_TCP`: tcp
- `IPPROTO_TPXX`: TP++ Transport
- `IPPROTO_TP`: tp-4 w
- `IPPROTO_TRUNK1`: Trunk-1
- `IPPROTO_TRUNK2`: Trunk-2
- `IPPROTO_TTP`: TTP
- `IPPROTO_UDP`: user datagram protocol
- `IPPROTO_VINES`: Banyon VINES
- `IPPROTO_VISA`: VISA Protocol
- `IPPROTO_VMTP`: VMTP
- `IPPROTO_WBEXPAK`: WIDEBAND EXPAK
- `IPPROTO_WBMON`: WIDEBAND Monitoring
- `IPPROTO_WSN`: Wang Span Network
- `IPPROTO_XNET`: Cross Net Debugger
- `IPPROTO_XTP`: XTP


### MSG_* Flags

- `MSG_BATCH`: sendmmsg(): more messages coming
- `MSG_CMSG_CLOEXEC`: Set close_on_exec for file descriptor received through SCM_RIGHTS
- `MSG_CONFIRM`: Confirm path validity
- `MSG_CTRUNC`: control data lost before delivery
- `MSG_DONTROUTE`: send without using routing tables
- `MSG_DONTWAIT`: this message should be nonblocking
- `MSG_EOF`: data completes connection
- `MSG_EOR`: data completes record
- `MSG_ERRQUEUE`: Fetch message from error queue
- `MSG_FASTOPEN`: Send data in TCP SYN
- `MSG_FLUSH`: Start of 'hold' seq; dump so_temp
- `MSG_HAVEMORE`: Data ready to be read
- `MSG_HOLD`: Hold frag in so_temp
- `MSG_MORE`: Sender will send more
- `MSG_NEEDSA`: Fail receive if socket address cannot be allocated
- `MSG_NOSIGNAL`: Do not generate SIGPIPE
- `MSG_OOB`: process out-of-band data
- `MSG_PEEK`: peek at incoming message
- `MSG_PROBE`: Do not send. Only probe path f.e. for MTU
- `MSG_RCVMORE`: Data remains in current pkt
- `MSG_SEND`: Send the packet in so_temp
- `MSG_TRUNC`: data discarded before delivery
- `MSG_TRYHARD`: Synonym for MSG_DONTROUTE for DECnet
- `MSG_WAITALL`: wait for full request or error
- `MSG_WAITFORONE`: recvmmsg(): block until 1+ packets avail
- `MSG_WAITSTREAM`: wait up to full request.. may return partial



### NI_* Flags

- `NI_DGRAM`
- `NI_NAMEREQD`
- `NI_NOFQDN`
- `NI_NUMERICHOST`
- `NI_NUMERICSERV`
- `NI_WITHSCOPEID`


### SOCK_* Types

- `SOCK_DGRAM`: datagram socket
- `SOCK_RAW`: raw-protocol interface
- `SOCK_RDM`: reliably-delivered message
- `SOCK_SEQPACKET`: sequenced packet stream
- `SOCK_STREAM`: stream socket


### SHUT_* Flags

- `SHUT_RD`: shut down the reading side
- `SHUT_WR`: shut down the writing side
- `SHUT_RDWR`: shut down both sides


***

# TODO

## Implementation

- sendmsg
- recvmsg

