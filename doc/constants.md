# Constants

these constants defined at the `llsocket.*`

[llsocket](../tmpl/llsocket_tmpl.c) exports the following constants.


## AF_* Types

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


## AI_* Flags

- `AI_ADDRCONFIG`: only if any address is assigned
- `AI_ALL`: IPv6 and IPv4-mapped (with AI_V4MAPPED)
- `AI_CANONNAME`: fill ai_canonname
- `AI_DEFAULT`: (AI_V4MAPPED_CFG | AI_ADDRCONFIG)
- `AI_NUMERICHOST`: prevent host name resolution
- `AI_NUMERICSERV`: prevent service name resolution
- `AI_PASSIVE`: get address to use bind()
- `AI_V4MAPPED`: accept IPv4-mapped IPv6 address
- `AI_V4MAPPED_CFG`: accept IPv4-mapped if kernel supports


## IPPROTO_* Types

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


## MSG_* Flags

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



## NI_* Flags

- `NI_DGRAM`
- `NI_NAMEREQD`
- `NI_NOFQDN`
- `NI_NUMERICHOST`
- `NI_NUMERICSERV`
- `NI_WITHSCOPEID`


## SOCK_* Types

- `SOCK_DGRAM`: datagram socket
- `SOCK_RAW`: raw-protocol interface
- `SOCK_RDM`: reliably-delivered message
- `SOCK_SEQPACKET`: sequenced packet stream
- `SOCK_STREAM`: stream socket


## SHUT_* Flags

- `SHUT_RD`: shut down the reading side
- `SHUT_WR`: shut down the writing side
- `SHUT_RDWR`: shut down both sides


## Socket Option Levels.

- `SOL_SOCKET`: options for socket level.


## Socket-level Control Message Types

- `SCM_CREDS`: process creds (struct cmsgcred)
- `SCM_RIGHTS`: access rights (array of int)
- `SCM_SECURIT`: security label
- `SCM_TIMESTAMP`: timestamp (struct timeval)
- `SCM_TIMESTAMP_MONOTONIC`: timestamp (uint64_t)


