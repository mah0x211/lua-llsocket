/*
 *  Copyright (C) 2014 Masatoshi Teruya
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 *
 *
 *  llsocket.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 14/03/29.
 *  Copyright 2014 Masatoshi Teruya. All rights reserved.
 *
 */

#include "llsocket.h"
#include <ifaddrs.h>
#include <net/if.h>
#if defined(__linux__)
    #include <sys/ioctl.h>
#else
    #include <net/if_dl.h>
#endif

#define DEFAULT_RECVSIZE    4096


// MARK: method
static int sockname_lua( lua_State *L )
{
    int fd = luaL_checkint( L, 1 );
    struct sockaddr_un *uaddr = NULL;
    struct sockaddr_in *iaddr = NULL;
    socklen_t len = sizeof( struct sockaddr_storage );
    struct sockaddr_storage addr;
    
    memset( (void*)&addr, 0, len );
    if( getsockname( fd, (void*)&addr, &len ) == 0 )
    {
        lua_newtable( L );
        switch( addr.ss_family ){
            case AF_INET:
                lstate_str2tbl( L, "family", "inet" );
                goto PUSH_INET_ADDR;
            case AF_UNIX:
                lstate_str2tbl( L, "family", "unix" );
                goto PUSH_UNIX_ADDR;
            default:
                lua_pop( L, 1 );
                errno = ENOTSUP;
                goto PUSH_ERROR;
        }

PUSH_INET_ADDR:
        iaddr = (struct sockaddr_in*)&addr;
        lstate_num2tbl( L, "port", ntohs( iaddr->sin_port ) );
        lstate_str2tbl( L, "addr", inet_ntoa( iaddr->sin_addr ) );
        return 1;
        
PUSH_UNIX_ADDR:
        uaddr = (struct sockaddr_un*)&addr;
        lstate_str2tbl( L, "path", uaddr->sun_path ); 
        return 1;
    }

PUSH_ERROR:
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


static int peername_lua( lua_State *L )
{
    int fd = luaL_checkint( L, 1 );
    socklen_t len = sizeof( struct sockaddr_storage );
    struct sockaddr_storage addr;
    
    memset( (void*)&addr, 0, len );
    if( getpeername( fd, (struct sockaddr*)&addr, &len ) == 0 &&
        // push llsocket.addr udata
        llsocket_addr_alloc( L, &addr, len ) == 0 ){
        return 1;
    }
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


static int atmark_lua( lua_State *L )
{
    int fd = luaL_checkint( L, 1 );
    int rc = sockatmark( fd );
    
    if( rc != -1 ){
        lua_pushboolean( L, rc );
        return 1;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


static int shutdown_lua( lua_State *L )
{
    int fd = luaL_checkint( L, 1 );
    int how = luaL_checkint( L, 2 );
    
    if( shutdown( fd, how ) == 0 ){
        // got error
        lua_pushstring( L, strerror( errno ) );
        return 1;
    }
    
    return 0;
}


static int close_lua( lua_State *L )
{
    int fd = luaL_checkint( L, 1 );
    
    if( fd )
    {
        int rc = 0;

        // check arguments
        if( !lua_isnoneornil( L, 2 ) ){
            int how = luaL_checkint( L, 2 );
            rc = shutdown( fd, how );
        }
        
        // got error
        if( ( rc + close( fd ) ) ){
            lua_pushstring( L, strerror( errno ) );
            return 1;
        }
    }
    
    return 0;
}


static int listen_lua( lua_State *L )
{
    int fd = luaL_checkint( L, 1 );
    // default backlog size
    lua_Integer backlog = luaL_optinteger( L, 2, SOMAXCONN );
    
    // check args
    if( backlog < 1 || backlog > INT_MAX ){
        return luaL_error( L, "backlog range must be 1 to %d", INT_MAX );
    }
    // listen
    else if( listen( fd, (int)backlog ) != 0 ){
        // got error
        lua_pushstring( L, strerror( errno ) );
        return 1;
    }
    
    return 0;
}


static int accept_lua( lua_State *L )
{
    int fd = luaL_checkint( L, 1 );
    int cfd = accept( fd, NULL, NULL );
    
    if( cfd != -1 )
    {
        if( fcntl( cfd, F_SETFD, FD_CLOEXEC ) != -1 ){
            lua_pushinteger( L, cfd );
            return 1;
        }
        
        close( cfd );
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    lua_pushboolean( L, errno == EAGAIN || errno == EWOULDBLOCK );
    
    return 3;
}


static int accept_inherits_lua( lua_State *L )
{
    int fd = luaL_checkint( L, 1 );
    int cfd = 0;

#if defined(__linux__)
    int flg = fcntl( cfd, F_GETFL );
    
    if( flg != -1 )
    {
#if defined(LINUX_ACCEPT4)
        flg = SOCK_CLOEXEC | ( ( flg & O_NONBLOCK ) ? SOCK_NONBLOCK : 0 );
        if( ( cfd = accept4( fd, NULL, NULL, flg ) ) != -1 ){
            lua_pushinteger( L, cfd );
            return 1;
        }
#else
        if( ( cfd = accept( fd, NULL, NULL ) ) != -1 )
        {
            if( fcntl( cfd, F_SETFD, FD_CLOEXEC ) == 0 &&
                fcntl( cfd, F_SETFL, flg ) == 0 ){
                lua_pushinteger( L, cfd );
                return 1;
            }
            close( cfd );
        }
#endif
    }

#else
    if( ( cfd = accept( fd, NULL, NULL ) ) != -1 ){
        lua_pushinteger( L, cfd );
        return 1;
    }
#endif

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    lua_pushboolean( L, errno == EAGAIN || errno == EWOULDBLOCK );
    
    return 3;
}


static int send_lua( lua_State *L )
{
    int fd = luaL_checkint( L, 1 );
    size_t len = 0; 
    const char *buf = luaL_checklstring( L, 2, &len );
    int flg = lls_optflags( L, 3 );
    ssize_t rv = send( fd, buf, len, flg );
    
    // got error
    if( rv == -1 ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        lua_pushboolean( L, errno == EAGAIN || errno == EWOULDBLOCK );
        rv = 3;
    }
    else {
        lua_pushinteger( L, rv );
        rv = 1;
    }
    
    return rv;
}


static int sendto_lua( lua_State *L )
{
    int fd = luaL_checkint( L, 1 );
    size_t len = 0; 
    const char *buf = luaL_checklstring( L, 2, &len );
    lls_addr_t *llsaddr = (lls_addr_t*)lls_checkudata( L, 3, LLS_ADDR_MT );
    int flg = lls_optflags( L, 4 );
    ssize_t rv = 0;
    
    if( llsaddr ){
        rv = sendto( fd, buf, len, flg, (const struct sockaddr*)&llsaddr->addr,
                     llsaddr->len );
    }
    else {
        rv = send( fd, buf, len, flg );
    }
    
    // got error
    if( rv == -1 ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        lua_pushboolean( L, errno == EAGAIN || errno == EWOULDBLOCK );
        rv = 3;
    }
    else {
        lua_pushinteger( L, rv );
        rv = 1;
    }
    
    return rv;
}


static int recv_lua( lua_State *L )
{
    int fd = luaL_checkint( L, 1 );
    lua_Integer len = luaL_optinteger( L, 2, DEFAULT_RECVSIZE );
    int flg = lls_optflags( L, 3 );
    char *buf = NULL;
    ssize_t rv = 0;
    
    // invalid length
    if( len <= 0 ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( EINVAL ) );
        return 2;
    }
    // mem-error
    else if( !( buf = pnalloc( len, char ) ) ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        return 2;
    }
    // got error
    else if( ( rv = recv( fd, buf, (size_t)len, flg ) ) == -1 ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        lua_pushboolean( L, errno == EAGAIN || errno == EWOULDBLOCK );
        rv = 3;
    }
    else {
        lua_pushlstring( L, buf, rv );
        rv = 1;
    }
    pdealloc( buf );
    
    return rv;
}


static int recvfrom_lua( lua_State *L )
{
    int fd = luaL_checkint( L, 1 );
    lua_Integer len = luaL_optinteger( L, 2, DEFAULT_RECVSIZE );
    int flg = lls_optflags( L, 3 );
    socklen_t slen = sizeof( struct sockaddr_storage );
    struct sockaddr_storage src;
    ssize_t rv = 0;
    char *buf = NULL;
    
    memset( (void*)&src, 0, slen );
    // invalid length
    if( len <= 0 ){
        lua_pushnil( L );
        lua_pushnil( L );
        lua_pushstring( L, strerror( EINVAL ) );
        return 3;
    }
    // mem-error
    else if( !( buf = pnalloc( len, char ) ) ){
        lua_pushnil( L );
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        return 3;
    }
    // got error
    else if( ( rv = recvfrom( fd, buf, (size_t)len, flg, 
             (struct sockaddr*)&src, &slen ) ) == -1 ){
        lua_pushnil( L );
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        lua_pushboolean( L, errno == EAGAIN || errno == EWOULDBLOCK );
        rv = 4;
    }
    else
    {
        lua_pushlstring( L, buf, rv );
        // no addrinfo
        if( slen == 0 ){
            rv = 1;
        }
        // push llsocket.addr udata
        else if( llsocket_addr_alloc( L, &src, slen ) == 0 ){
            rv = 2;
        }
        else {
            lua_pop( L, 1 );
            lua_pushnil( L );
            lua_pushnil( L );
            lua_pushstring( L, strerror( errno ) );
            rv = 3;
        }
    }
    pdealloc( buf );
    
    return rv;
}


// MARK: fd option
static int fcntl_lua( lua_State *L, int getfl, int setfl, int fl )
{
    int fd = luaL_checkint( L, 1 );
    
    return lls_fcntl_lua( L, fd, getfl, setfl, fl );
}


static int cloexec_lua( lua_State *L )
{
    return fcntl_lua( L, F_GETFD, F_SETFD, FD_CLOEXEC );
}


static int nonblock_lua( lua_State *L )
{
    return fcntl_lua( L, F_GETFL, F_SETFL, O_NONBLOCK );
}


// MARK: socket option
#define sockopt_int(L,level,optname,type,optrw) ({ \
    int fd = luaL_checkint( L, 1 ); \
    lls_sockopt_int_lua( L, fd, level, optname, type, optrw ); \
});
#define sockopt_int_lua(L,level,optname,type) \
    sockopt_int(L,level,optname,type,LLS_SOCKOPT_WRITE)

#define sockopt_readint_lua(L,level,optname,type) \
    sockopt_int(L,level,optname,type,LLS_SOCKOPT_READ)

// readonly
static int type_lua( lua_State *L )
{
    return sockopt_readint_lua( L, SOL_SOCKET, SO_TYPE, LUA_TNUMBER );
}

static int error_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_ERROR, LUA_TNUMBER );
}


// writable
static int nodelay_lua( lua_State *L )
{
    return sockopt_int_lua( L, IPPROTO_TCP, TCP_NODELAY, LUA_TBOOLEAN );
}

static int reuseaddr_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_REUSEADDR, LUA_TBOOLEAN );
}

static int broadcast_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_BROADCAST, LUA_TBOOLEAN );
}

static int debug_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_DEBUG, LUA_TBOOLEAN );
}

static int keepalive_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_KEEPALIVE, LUA_TBOOLEAN );
}

static int oobinline_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_OOBINLINE, LUA_TBOOLEAN );
}

static int timestamp_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_TIMESTAMP, LUA_TBOOLEAN );
}

static int rcvbuf_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_RCVBUF, LUA_TNUMBER );
}

static int rcvlowat_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_RCVLOWAT, LUA_TNUMBER );
}

static int sndbuf_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_SNDBUF, LUA_TNUMBER );
}

static int sndlowat_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_SNDLOWAT, LUA_TNUMBER );
}


static int sockopt_timeval_lua( lua_State *L, int level, int opt )
{
    int fd = luaL_checkint( L, 1 );
    
    return lls_sockopt_timeval_lua( L, fd, level, opt );
}

static int rcvtimeo_lua( lua_State *L )
{
    return sockopt_timeval_lua( L, SOL_SOCKET, SO_RCVTIMEO );
}

static int sndtimeo_lua( lua_State *L )
{
    return sockopt_timeval_lua( L, SOL_SOCKET, SO_SNDTIMEO );
}


// MARK: device info
#if defined(__linux__)
static int macaddrs_lua( lua_State *L )
{
    int fd = socket( AF_INET, SOCK_DGRAM, 0 );

    if( fd )
    {
        char buf[INET6_ADDRSTRLEN];
        struct ifreq ifrbuf[16];
        struct ifconf ifc = {
            .ifc_len = sizeof( ifrbuf ),
            .ifc_req = ifrbuf
        };

        // get if-conf
        if( ioctl( fd, SIOCGIFCONF, &ifc ) != -1 )
        {
            struct ifreq* it = ifc.ifc_req;
            struct ifreq* end = it + ( ifc.ifc_len / sizeof( struct ifreq ) );
            unsigned char *mac = NULL;

            lua_newtable( L );
            for(; it != end; it++ )
            {
                    // inet only
                if( it->ifr_addr.sa_family == AF_INET &&
                    // running only and ignore loopback
                    ioctl( fd, SIOCGIFFLAGS, it ) != -1 &&
                    it->ifr_flags & IFF_RUNNING &&
                    !( it->ifr_flags & IFF_LOOPBACK ) &&
                    // get flags and hardware address
                    ioctl( fd, SIOCGIFHWADDR, it ) != -1 ){
                    mac = (unsigned char*)it->ifr_hwaddr.sa_data;

                    snprintf( buf, INET6_ADDRSTRLEN,
                        "%02x:%02x:%02x:%02x:%02x:%02x",
                        *mac, mac[1], mac[2], mac[3], mac[4], mac[5]
                    );
                    lstate_str2tbl( L, it->ifr_name, buf );
                }
            }

            close( fd );
            return 1;

        }

        close( fd );
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


#else
static int macaddrs_lua( lua_State *L )
{
    struct ifaddrs *ifa;
    
    if( getifaddrs( &ifa ) == 0 )
    {
        lua_newtable( L );
        
        if( ifa )
        {
            struct ifaddrs *ptr = ifa;
            char buf[INET6_ADDRSTRLEN];
            unsigned char *mac = NULL;
            struct sockaddr_dl *sd;
            
            do
            {
                if( ptr->ifa_addr->sa_family == AF_LINK )
                {
                    sd = (struct sockaddr_dl*)ptr->ifa_addr;
                    switch( sd->sdl_alen ){
                        case 6:
                            mac = (unsigned char*)LLADDR( sd );
                            snprintf( 
                                buf, INET6_ADDRSTRLEN, 
                                "%02x:%02x:%02x:%02x:%02x:%02x",
                                *mac, mac[1], mac[2], mac[3], mac[4], mac[5] 
                            );
                            lstate_str2tbl( L, ptr->ifa_name, buf );
                        break;
                        case 8:
                            mac = (unsigned char*)LLADDR( sd );
                            snprintf( 
                                buf, INET6_ADDRSTRLEN, 
                                "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
                                *mac, mac[1], mac[2], mac[3], mac[4], mac[5], 
                                mac[6], mac[7]
                            );
                            lstate_str2tbl( L, ptr->ifa_name, buf );
                        break;
                    }
                }
                
                ptr = ptr->ifa_next;
            } while( ptr );
        }
        
        freeifaddrs( ifa );
        return 1;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}
#endif


LUALIB_API int luaopen_llsocket( lua_State *L )
{
    struct luaL_Reg method[] = {
        // with socket-fd
        // method
        { "sockname", sockname_lua },
        { "peername", peername_lua },
        { "atmark", atmark_lua },
        { "type", type_lua },
        { "error", error_lua },
        { "shutdown", shutdown_lua },
        { "close", close_lua },
        { "listen", listen_lua },
        { "accept", accept_lua },
        { "acceptInherits", accept_inherits_lua },
        { "send", send_lua },
        { "sendto", sendto_lua },
        { "recv", recv_lua },
        { "recvfrom", recvfrom_lua },
        { NULL, NULL }
    };
    struct luaL_Reg opt_method[] = {
        // fd option
        { "cloexec", cloexec_lua },
        { "nonblock", nonblock_lua },
        // socket option
        { "nodelay", nodelay_lua },
        { "reuseaddr", reuseaddr_lua },
        { "broadcast", broadcast_lua },
        { "debug", debug_lua },
        { "keepalive", keepalive_lua },
        { "oobinline", oobinline_lua },
        { "timestamp", timestamp_lua },
        { "rcvbuf", rcvbuf_lua },
        { "rcvlowat", rcvlowat_lua },
        { "sndbuf", sndbuf_lua },
        { "sndlowat", sndlowat_lua },
        { "rcvtimeo", rcvtimeo_lua },
        { "sndtimeo", sndtimeo_lua },
        { NULL, NULL }
    };
    struct luaL_Reg device_method[] = {
        // device info
        { "macaddrs", macaddrs_lua },
        { NULL, NULL }
    };
    struct luaL_Reg *ptr = NULL;
    
    // create table
    lua_newtable( L );
    // open libs
    lua_pushstring( L, "inet" );
    luaopen_llsocket_inet( L );
    lua_rawset( L, -3 );
    lua_pushstring( L, "unix" );
    luaopen_llsocket_unix( L );
    lua_rawset( L, -3 );
    // no alloc interface
    luaopen_llsocket_addr( L );

    // method
    ptr = method;
    do {
        lstate_fn2tbl( L, ptr->name, ptr->func );
        ptr++;
    } while( ptr->name );
    
    // device method
    lua_pushstring( L, "device" );
    lua_newtable( L );
    ptr = device_method;
    do {
        lstate_fn2tbl( L, ptr->name, ptr->func );
        ptr++;
    } while( ptr->name );
    lua_rawset( L, -3 );
    
    // option method
    lua_pushstring( L, "opt" );
    lua_newtable( L );
    ptr = opt_method;
    do {
        lstate_fn2tbl( L, ptr->name, ptr->func );
        ptr++;
    } while( ptr->name );
    // constants
    // for connect and bind
    lstate_num2tbl( L, "SOCK_STREAM", SOCK_STREAM );
    lstate_num2tbl( L, "SOCK_DGRAM", SOCK_DGRAM );
    lstate_num2tbl( L, "SOCK_SEQPACKET", SOCK_SEQPACKET );
    lstate_num2tbl( L, "SOCK_RAW", SOCK_RAW );
    // for shutdown
    lstate_num2tbl( L, "SHUT_RD", SHUT_RD );
    lstate_num2tbl( L, "SHUT_WR", SHUT_WR );
    lstate_num2tbl( L, "SHUT_RDWR", SHUT_RDWR );
    // for send/recv flags
#define GEN_SENDRECV_FLAGS_DECL
    
    lua_rawset( L, -3 );
    
    return 1;
}


