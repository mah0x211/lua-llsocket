/*
 *  Copyright (C) 2014 Masatoshi Teruya
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a 
 *  copy of this software and associated documentation files (the "Software"), 
 *  to deal in the Software without restriction, including without limitation 
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 *  and/or sell copies of the Software, and to permit persons to whom the 
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.
 *
 *
 *  llsocket.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 14/03/29.
 */

#include "llsocket.h"


LUALIB_API int luaopen_llsocket( lua_State *L )
{
    // register submodule
    luaopen_llsocket_addrinfo( L );

    // create table
    lua_newtable( L );
    // add submodules
    lua_pushstring( L, "socket" );
    luaopen_llsocket_socket( L );
    lua_rawset( L, -3 );

    lua_pushstring( L, "inet" );
    luaopen_llsocket_inet( L );
    lua_rawset( L, -3 );
    
    lua_pushstring( L, "unix" );
    luaopen_llsocket_unix( L );
    lua_rawset( L, -3 );
    
    lua_pushstring( L, "device" );
    luaopen_llsocket_device( L );
    lua_rawset( L, -3 );

    // constants
    // for shutdown
    lstate_num2tbl( L, "SHUT_RD", SHUT_RD );
    lstate_num2tbl( L, "SHUT_WR", SHUT_WR );
    lstate_num2tbl( L, "SHUT_RDWR", SHUT_RDWR );

    // for send/recv flags
#ifdef MSG_CMSG_CLOEXEC
    lstate_num2tbl( L, "MSG_CMSG_CLOEXEC", MSG_CMSG_CLOEXEC );
#endif

#ifdef MSG_CONFIRM
    lstate_num2tbl( L, "MSG_CONFIRM", MSG_CONFIRM );
#endif

#ifdef MSG_CTRUNC
    lstate_num2tbl( L, "MSG_CTRUNC", MSG_CTRUNC );
#endif

#ifdef MSG_DONTROUTE
    lstate_num2tbl( L, "MSG_DONTROUTE", MSG_DONTROUTE );
#endif

#ifdef MSG_DONTWAIT
    lstate_num2tbl( L, "MSG_DONTWAIT", MSG_DONTWAIT );
#endif

#ifdef MSG_EOR
    lstate_num2tbl( L, "MSG_EOR", MSG_EOR );
#endif

#ifdef MSG_ERRQUEUE
    lstate_num2tbl( L, "MSG_ERRQUEUE", MSG_ERRQUEUE );
#endif

#ifdef MSG_MORE
    lstate_num2tbl( L, "MSG_MORE", MSG_MORE );
#endif

#ifdef MSG_NOSIGNAL
    lstate_num2tbl( L, "MSG_NOSIGNAL", MSG_NOSIGNAL );
#endif

#ifdef MSG_OOB
    lstate_num2tbl( L, "MSG_OOB", MSG_OOB );
#endif

#ifdef MSG_PEEK
    lstate_num2tbl( L, "MSG_PEEK", MSG_PEEK );
#endif

#ifdef MSG_TRUNC
    lstate_num2tbl( L, "MSG_TRUNC", MSG_TRUNC );
#endif

#ifdef MSG_WAITALL
    lstate_num2tbl( L, "MSG_WAITALL", MSG_WAITALL );
#endif

    // socktype
#ifdef SOCK_DGRAM
    lstate_num2tbl( L, "SOCK_DGRAM", SOCK_DGRAM );
#endif

#ifdef SOCK_RAW
    lstate_num2tbl( L, "SOCK_RAW", SOCK_RAW );
#endif

#ifdef SOCK_RDM
    lstate_num2tbl( L, "SOCK_RDM", SOCK_RDM );
#endif

#ifdef SOCK_SEQPACKET
    lstate_num2tbl( L, "SOCK_SEQPACKET", SOCK_SEQPACKET );
#endif

#ifdef SOCK_STREAM
    lstate_num2tbl( L, "SOCK_STREAM", SOCK_STREAM );
#endif

    // protocol
#ifdef IPPROTO_3PC
    lstate_num2tbl( L, "IPPROTO_3PC", IPPROTO_3PC );
#endif

#ifdef IPPROTO_ADFS
    lstate_num2tbl( L, "IPPROTO_ADFS", IPPROTO_ADFS );
#endif

#ifdef IPPROTO_AH
    lstate_num2tbl( L, "IPPROTO_AH", IPPROTO_AH );
#endif

#ifdef IPPROTO_AHIP
    lstate_num2tbl( L, "IPPROTO_AHIP", IPPROTO_AHIP );
#endif

#ifdef IPPROTO_APES
    lstate_num2tbl( L, "IPPROTO_APES", IPPROTO_APES );
#endif

#ifdef IPPROTO_ARGUS
    lstate_num2tbl( L, "IPPROTO_ARGUS", IPPROTO_ARGUS );
#endif

#ifdef IPPROTO_AX25
    lstate_num2tbl( L, "IPPROTO_AX25", IPPROTO_AX25 );
#endif

#ifdef IPPROTO_BHA
    lstate_num2tbl( L, "IPPROTO_BHA", IPPROTO_BHA );
#endif

#ifdef IPPROTO_BLT
    lstate_num2tbl( L, "IPPROTO_BLT", IPPROTO_BLT );
#endif

#ifdef IPPROTO_BRSATMON
    lstate_num2tbl( L, "IPPROTO_BRSATMON", IPPROTO_BRSATMON );
#endif

#ifdef IPPROTO_CFTP
    lstate_num2tbl( L, "IPPROTO_CFTP", IPPROTO_CFTP );
#endif

#ifdef IPPROTO_CHAOS
    lstate_num2tbl( L, "IPPROTO_CHAOS", IPPROTO_CHAOS );
#endif

#ifdef IPPROTO_CMTP
    lstate_num2tbl( L, "IPPROTO_CMTP", IPPROTO_CMTP );
#endif

#ifdef IPPROTO_CPHB
    lstate_num2tbl( L, "IPPROTO_CPHB", IPPROTO_CPHB );
#endif

#ifdef IPPROTO_CPNX
    lstate_num2tbl( L, "IPPROTO_CPNX", IPPROTO_CPNX );
#endif

#ifdef IPPROTO_DDP
    lstate_num2tbl( L, "IPPROTO_DDP", IPPROTO_DDP );
#endif

#ifdef IPPROTO_DGP
    lstate_num2tbl( L, "IPPROTO_DGP", IPPROTO_DGP );
#endif

#ifdef IPPROTO_DIVERT
    lstate_num2tbl( L, "IPPROTO_DIVERT", IPPROTO_DIVERT );
#endif

#ifdef IPPROTO_DSTOPTS
    lstate_num2tbl( L, "IPPROTO_DSTOPTS", IPPROTO_DSTOPTS );
#endif

#ifdef IPPROTO_EGP
    lstate_num2tbl( L, "IPPROTO_EGP", IPPROTO_EGP );
#endif

#ifdef IPPROTO_EMCON
    lstate_num2tbl( L, "IPPROTO_EMCON", IPPROTO_EMCON );
#endif

#ifdef IPPROTO_ENCAP
    lstate_num2tbl( L, "IPPROTO_ENCAP", IPPROTO_ENCAP );
#endif

#ifdef IPPROTO_EON
    lstate_num2tbl( L, "IPPROTO_EON", IPPROTO_EON );
#endif

#ifdef IPPROTO_ESP
    lstate_num2tbl( L, "IPPROTO_ESP", IPPROTO_ESP );
#endif

#ifdef IPPROTO_ETHERIP
    lstate_num2tbl( L, "IPPROTO_ETHERIP", IPPROTO_ETHERIP );
#endif

#ifdef IPPROTO_FRAGMENT
    lstate_num2tbl( L, "IPPROTO_FRAGMENT", IPPROTO_FRAGMENT );
#endif

#ifdef IPPROTO_GGP
    lstate_num2tbl( L, "IPPROTO_GGP", IPPROTO_GGP );
#endif

#ifdef IPPROTO_GMTP
    lstate_num2tbl( L, "IPPROTO_GMTP", IPPROTO_GMTP );
#endif

#ifdef IPPROTO_GRE
    lstate_num2tbl( L, "IPPROTO_GRE", IPPROTO_GRE );
#endif

#ifdef IPPROTO_HELLO
    lstate_num2tbl( L, "IPPROTO_HELLO", IPPROTO_HELLO );
#endif

#ifdef IPPROTO_HMP
    lstate_num2tbl( L, "IPPROTO_HMP", IPPROTO_HMP );
#endif

#ifdef IPPROTO_HOPOPTS
    lstate_num2tbl( L, "IPPROTO_HOPOPTS", IPPROTO_HOPOPTS );
#endif

#ifdef IPPROTO_ICMP
    lstate_num2tbl( L, "IPPROTO_ICMP", IPPROTO_ICMP );
#endif

#ifdef IPPROTO_ICMPV6
    lstate_num2tbl( L, "IPPROTO_ICMPV6", IPPROTO_ICMPV6 );
#endif

#ifdef IPPROTO_IDP
    lstate_num2tbl( L, "IPPROTO_IDP", IPPROTO_IDP );
#endif

#ifdef IPPROTO_IDPR
    lstate_num2tbl( L, "IPPROTO_IDPR", IPPROTO_IDPR );
#endif

#ifdef IPPROTO_IDRP
    lstate_num2tbl( L, "IPPROTO_IDRP", IPPROTO_IDRP );
#endif

#ifdef IPPROTO_IGMP
    lstate_num2tbl( L, "IPPROTO_IGMP", IPPROTO_IGMP );
#endif

#ifdef IPPROTO_IGP
    lstate_num2tbl( L, "IPPROTO_IGP", IPPROTO_IGP );
#endif

#ifdef IPPROTO_IGRP
    lstate_num2tbl( L, "IPPROTO_IGRP", IPPROTO_IGRP );
#endif

#ifdef IPPROTO_IL
    lstate_num2tbl( L, "IPPROTO_IL", IPPROTO_IL );
#endif

#ifdef IPPROTO_INLSP
    lstate_num2tbl( L, "IPPROTO_INLSP", IPPROTO_INLSP );
#endif

#ifdef IPPROTO_INP
    lstate_num2tbl( L, "IPPROTO_INP", IPPROTO_INP );
#endif

#ifdef IPPROTO_IP
    lstate_num2tbl( L, "IPPROTO_IP", IPPROTO_IP );
#endif

#ifdef IPPROTO_IPCOMP
    lstate_num2tbl( L, "IPPROTO_IPCOMP", IPPROTO_IPCOMP );
#endif

#ifdef IPPROTO_IPCV
    lstate_num2tbl( L, "IPPROTO_IPCV", IPPROTO_IPCV );
#endif

#ifdef IPPROTO_IPEIP
    lstate_num2tbl( L, "IPPROTO_IPEIP", IPPROTO_IPEIP );
#endif

#ifdef IPPROTO_IPIP
    lstate_num2tbl( L, "IPPROTO_IPIP", IPPROTO_IPIP );
#endif

#ifdef IPPROTO_IPPC
    lstate_num2tbl( L, "IPPROTO_IPPC", IPPROTO_IPPC );
#endif

#ifdef IPPROTO_IPV4
    lstate_num2tbl( L, "IPPROTO_IPV4", IPPROTO_IPV4 );
#endif

#ifdef IPPROTO_IPV6
    lstate_num2tbl( L, "IPPROTO_IPV6", IPPROTO_IPV6 );
#endif

#ifdef IPPROTO_IRTP
    lstate_num2tbl( L, "IPPROTO_IRTP", IPPROTO_IRTP );
#endif

#ifdef IPPROTO_KRYPTOLAN
    lstate_num2tbl( L, "IPPROTO_KRYPTOLAN", IPPROTO_KRYPTOLAN );
#endif

#ifdef IPPROTO_LARP
    lstate_num2tbl( L, "IPPROTO_LARP", IPPROTO_LARP );
#endif

#ifdef IPPROTO_LEAF1
    lstate_num2tbl( L, "IPPROTO_LEAF1", IPPROTO_LEAF1 );
#endif

#ifdef IPPROTO_LEAF2
    lstate_num2tbl( L, "IPPROTO_LEAF2", IPPROTO_LEAF2 );
#endif

#ifdef IPPROTO_MEAS
    lstate_num2tbl( L, "IPPROTO_MEAS", IPPROTO_MEAS );
#endif

#ifdef IPPROTO_MHRP
    lstate_num2tbl( L, "IPPROTO_MHRP", IPPROTO_MHRP );
#endif

#ifdef IPPROTO_MICP
    lstate_num2tbl( L, "IPPROTO_MICP", IPPROTO_MICP );
#endif

#ifdef IPPROTO_MTP
    lstate_num2tbl( L, "IPPROTO_MTP", IPPROTO_MTP );
#endif

#ifdef IPPROTO_MUX
    lstate_num2tbl( L, "IPPROTO_MUX", IPPROTO_MUX );
#endif

#ifdef IPPROTO_ND
    lstate_num2tbl( L, "IPPROTO_ND", IPPROTO_ND );
#endif

#ifdef IPPROTO_NHRP
    lstate_num2tbl( L, "IPPROTO_NHRP", IPPROTO_NHRP );
#endif

#ifdef IPPROTO_NONE
    lstate_num2tbl( L, "IPPROTO_NONE", IPPROTO_NONE );
#endif

#ifdef IPPROTO_NSP
    lstate_num2tbl( L, "IPPROTO_NSP", IPPROTO_NSP );
#endif

#ifdef IPPROTO_NVPII
    lstate_num2tbl( L, "IPPROTO_NVPII", IPPROTO_NVPII );
#endif

#ifdef IPPROTO_OSPFIGP
    lstate_num2tbl( L, "IPPROTO_OSPFIGP", IPPROTO_OSPFIGP );
#endif

#ifdef IPPROTO_PGM
    lstate_num2tbl( L, "IPPROTO_PGM", IPPROTO_PGM );
#endif

#ifdef IPPROTO_PIGP
    lstate_num2tbl( L, "IPPROTO_PIGP", IPPROTO_PIGP );
#endif

#ifdef IPPROTO_PIM
    lstate_num2tbl( L, "IPPROTO_PIM", IPPROTO_PIM );
#endif

#ifdef IPPROTO_PRM
    lstate_num2tbl( L, "IPPROTO_PRM", IPPROTO_PRM );
#endif

#ifdef IPPROTO_PUP
    lstate_num2tbl( L, "IPPROTO_PUP", IPPROTO_PUP );
#endif

#ifdef IPPROTO_PVP
    lstate_num2tbl( L, "IPPROTO_PVP", IPPROTO_PVP );
#endif

#ifdef IPPROTO_RAW
    lstate_num2tbl( L, "IPPROTO_RAW", IPPROTO_RAW );
#endif

#ifdef IPPROTO_RCCMON
    lstate_num2tbl( L, "IPPROTO_RCCMON", IPPROTO_RCCMON );
#endif

#ifdef IPPROTO_RDP
    lstate_num2tbl( L, "IPPROTO_RDP", IPPROTO_RDP );
#endif

#ifdef IPPROTO_ROUTING
    lstate_num2tbl( L, "IPPROTO_ROUTING", IPPROTO_ROUTING );
#endif

#ifdef IPPROTO_RSVP
    lstate_num2tbl( L, "IPPROTO_RSVP", IPPROTO_RSVP );
#endif

#ifdef IPPROTO_RVD
    lstate_num2tbl( L, "IPPROTO_RVD", IPPROTO_RVD );
#endif

#ifdef IPPROTO_SATEXPAK
    lstate_num2tbl( L, "IPPROTO_SATEXPAK", IPPROTO_SATEXPAK );
#endif

#ifdef IPPROTO_SATMON
    lstate_num2tbl( L, "IPPROTO_SATMON", IPPROTO_SATMON );
#endif

#ifdef IPPROTO_SCCSP
    lstate_num2tbl( L, "IPPROTO_SCCSP", IPPROTO_SCCSP );
#endif

#ifdef IPPROTO_SCTP
    lstate_num2tbl( L, "IPPROTO_SCTP", IPPROTO_SCTP );
#endif

#ifdef IPPROTO_SDRP
    lstate_num2tbl( L, "IPPROTO_SDRP", IPPROTO_SDRP );
#endif

#ifdef IPPROTO_SEP
    lstate_num2tbl( L, "IPPROTO_SEP", IPPROTO_SEP );
#endif

#ifdef IPPROTO_SRPC
    lstate_num2tbl( L, "IPPROTO_SRPC", IPPROTO_SRPC );
#endif

#ifdef IPPROTO_ST
    lstate_num2tbl( L, "IPPROTO_ST", IPPROTO_ST );
#endif

#ifdef IPPROTO_SVMTP
    lstate_num2tbl( L, "IPPROTO_SVMTP", IPPROTO_SVMTP );
#endif

#ifdef IPPROTO_SWIPE
    lstate_num2tbl( L, "IPPROTO_SWIPE", IPPROTO_SWIPE );
#endif

#ifdef IPPROTO_TCF
    lstate_num2tbl( L, "IPPROTO_TCF", IPPROTO_TCF );
#endif

#ifdef IPPROTO_TCP
    lstate_num2tbl( L, "IPPROTO_TCP", IPPROTO_TCP );
#endif

#ifdef IPPROTO_TP
    lstate_num2tbl( L, "IPPROTO_TP", IPPROTO_TP );
#endif

#ifdef IPPROTO_TPXX
    lstate_num2tbl( L, "IPPROTO_TPXX", IPPROTO_TPXX );
#endif

#ifdef IPPROTO_TRUNK1
    lstate_num2tbl( L, "IPPROTO_TRUNK1", IPPROTO_TRUNK1 );
#endif

#ifdef IPPROTO_TRUNK2
    lstate_num2tbl( L, "IPPROTO_TRUNK2", IPPROTO_TRUNK2 );
#endif

#ifdef IPPROTO_TTP
    lstate_num2tbl( L, "IPPROTO_TTP", IPPROTO_TTP );
#endif

#ifdef IPPROTO_UDP
    lstate_num2tbl( L, "IPPROTO_UDP", IPPROTO_UDP );
#endif

#ifdef IPPROTO_VINES
    lstate_num2tbl( L, "IPPROTO_VINES", IPPROTO_VINES );
#endif

#ifdef IPPROTO_VISA
    lstate_num2tbl( L, "IPPROTO_VISA", IPPROTO_VISA );
#endif

#ifdef IPPROTO_VMTP
    lstate_num2tbl( L, "IPPROTO_VMTP", IPPROTO_VMTP );
#endif

#ifdef IPPROTO_WBEXPAK
    lstate_num2tbl( L, "IPPROTO_WBEXPAK", IPPROTO_WBEXPAK );
#endif

#ifdef IPPROTO_WBMON
    lstate_num2tbl( L, "IPPROTO_WBMON", IPPROTO_WBMON );
#endif

#ifdef IPPROTO_WSN
    lstate_num2tbl( L, "IPPROTO_WSN", IPPROTO_WSN );
#endif

#ifdef IPPROTO_XNET
    lstate_num2tbl( L, "IPPROTO_XNET", IPPROTO_XNET );
#endif

#ifdef IPPROTO_XTP
    lstate_num2tbl( L, "IPPROTO_XTP", IPPROTO_XTP );
#endif

    // ai_flags
#ifdef AI_ADDRCONFIG
    lstate_num2tbl( L, "AI_ADDRCONFIG", AI_ADDRCONFIG );
#endif

#ifdef AI_ALL
    lstate_num2tbl( L, "AI_ALL", AI_ALL );
#endif

#ifdef AI_CANONNAME
    lstate_num2tbl( L, "AI_CANONNAME", AI_CANONNAME );
#endif

#ifdef AI_DEFAULT
    lstate_num2tbl( L, "AI_DEFAULT", AI_DEFAULT );
#endif

#ifdef AI_NUMERICHOST
    lstate_num2tbl( L, "AI_NUMERICHOST", AI_NUMERICHOST );
#endif

#ifdef AI_NUMERICSERV
    lstate_num2tbl( L, "AI_NUMERICSERV", AI_NUMERICSERV );
#endif

#ifdef AI_PASSIVE
    lstate_num2tbl( L, "AI_PASSIVE", AI_PASSIVE );
#endif

#ifdef AI_V4MAPPED
    lstate_num2tbl( L, "AI_V4MAPPED", AI_V4MAPPED );
#endif

#ifdef AI_V4MAPPED_CFG
    lstate_num2tbl( L, "AI_V4MAPPED_CFG", AI_V4MAPPED_CFG );
#endif

    // ni_flags
#ifdef NI_DGRAM
    lstate_num2tbl( L, "NI_DGRAM", NI_DGRAM );
#endif

#ifdef NI_NAMEREQD
    lstate_num2tbl( L, "NI_NAMEREQD", NI_NAMEREQD );
#endif

#ifdef NI_NOFQDN
    lstate_num2tbl( L, "NI_NOFQDN", NI_NOFQDN );
#endif

#ifdef NI_NUMERICHOST
    lstate_num2tbl( L, "NI_NUMERICHOST", NI_NUMERICHOST );
#endif

#ifdef NI_NUMERICSERV
    lstate_num2tbl( L, "NI_NUMERICSERV", NI_NUMERICSERV );
#endif

#ifdef NI_WITHSCOPEID
    lstate_num2tbl( L, "NI_WITHSCOPEID", NI_WITHSCOPEID );
#endif


    
    return 1;
}


