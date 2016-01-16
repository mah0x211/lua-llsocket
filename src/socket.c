/*
 *  Copyright (C) 2015 Masatoshi Teruya
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
 *  socket.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 15/12/17.
 */

#include "llsocket.h"

#define DEFAULT_RECVSIZE    4096

typedef struct {
    int fd;
    int family;
    int socktype;
    int protocol;
    socklen_t addrlen;
    struct sockaddr_storage addr;
} lls_socket_t;



// MARK: fd option
#define fcntl_lua(L,getfl,setfl,fl) ({ \
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT ); \
    lls_fcntl_lua( L, s->fd, getfl, setfl, fl ); \
})


static int cloexec_lua( lua_State *L )
{
    return fcntl_lua( L, F_GETFD, F_SETFD, FD_CLOEXEC );
}


static int nonblock_lua( lua_State *L )
{
    return fcntl_lua( L, F_GETFL, F_SETFL, O_NONBLOCK );
}


// MARK: socket option
#define sockopt_int_lua(L,level,optname,type) ({ \
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT ); \
    lls_sockopt_int_lua( L, s->fd, level, optname, type ); \
})


// readonly

static int error_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_ERROR, LUA_TNUMBER );
}

static int acceptconn_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_ACCEPTCONN, LUA_TBOOLEAN );
}


// writable
static int tcpnodelay_lua( lua_State *L )
{
    return sockopt_int_lua( L, IPPROTO_TCP, TCP_NODELAY, LUA_TBOOLEAN );
}

static int tcpkeepintvl_lua( lua_State *L )
{
    return sockopt_int_lua( L, IPPROTO_TCP, TCP_KEEPINTVL, LUA_TNUMBER );
}

static int tcpkeepcnt_lua( lua_State *L )
{
    return sockopt_int_lua( L, IPPROTO_TCP, TCP_KEEPCNT, LUA_TNUMBER );
}

#if defined(TCP_KEEPALIVE) || defined(TCP_KEEPIDLE)
#define HAVE_TCP_KEEPALIVE

static int tcpkeepalive_lua( lua_State *L )
{
#if defined(TCP_KEEPALIVE)
    return sockopt_int_lua( L, IPPROTO_TCP, TCP_KEEPALIVE, LUA_TNUMBER );
#else
    return sockopt_int_lua( L, IPPROTO_TCP, TCP_KEEPIDLE, LUA_TNUMBER );
#endif
}

#endif


#if defined(TCP_CORK) || defined(TCP_NOPUSH)
#define HAVE_TCP_CORK 1

static int tcpcork_lua( lua_State *L )
{
#if defined(TCP_CORK)
    return sockopt_int_lua( L, IPPROTO_TCP, TCP_CORK, LUA_TBOOLEAN );
#else
    return sockopt_int_lua( L, IPPROTO_TCP, TCP_NOPUSH, LUA_TBOOLEAN );
#endif
}

#endif

#if defined(SO_REUSEPORT)
#define HAVE_SO_REUSEPORT 1

static int reuseport_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_REUSEPORT, LUA_TBOOLEAN );
}

#endif

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


#define sockopt_timeval_lua(L,level,opt) ({ \
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT ); \
    lls_sockopt_timeval_lua( L, s->fd, level, opt ); \
})

static int rcvtimeo_lua( lua_State *L )
{
    return sockopt_timeval_lua( L, SOL_SOCKET, SO_RCVTIMEO );
}

static int sndtimeo_lua( lua_State *L )
{
    return sockopt_timeval_lua( L, SOL_SOCKET, SO_SNDTIMEO );
}


// MARK: state

static int atmark_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );
    int rc = sockatmark( s->fd );
    
    if( rc != -1 ){
        lua_pushboolean( L, rc );
        return 1;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


// MARK: address info

static int getsockname_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );
    struct addrinfo wrap = {
        .ai_flags = 0,
        .ai_family = s->family,
        .ai_socktype = s->socktype,
        .ai_protocol = s->protocol,
        .ai_addrlen = s->addrlen,
        .ai_addr = (struct sockaddr*)&s->addr,
        .ai_canonname = NULL,
        .ai_next = NULL
    };

    // push llsocket.addr udata
    if( lls_addrinfo_alloc( L, &wrap ) ){
        return 1;
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


static int getpeername_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );
    socklen_t len = sizeof( struct sockaddr_storage );
    struct sockaddr_storage addr;

    memset( (void*)&addr, 0, len );
    if( getpeername( s->fd, (struct sockaddr*)&addr, &len ) == 0 )
    {
        struct addrinfo wrap = {
            .ai_flags = 0,
            .ai_family = s->family,
            .ai_socktype = s->socktype,
            .ai_protocol = s->protocol,
            .ai_addrlen = len,
            .ai_addr = (struct sockaddr*)&addr,
            .ai_canonname = NULL,
            .ai_next = NULL
        };

        if( lls_addrinfo_alloc( L, &wrap ) ){
            return 1;
        }
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


// MARK: method

static int shutdown_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );
    int how = (int)lls_checkinteger( L, 2 );
    
    if( shutdown( s->fd, how ) == 0 ){
        return 0;
    }

    // got error
    lua_pushstring( L, strerror( errno ) );

    return 1;
}


static int close_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );

    if( s->fd != -1 )
    {
        int how = (int)lls_optinteger( L, 2, -1 );
        int fd = s->fd;
        int rc = 0;

        s->fd = -1;
        switch( how ){
            case SHUT_RD:
            case SHUT_WR:
            case SHUT_RDWR:
                rc = shutdown( fd, how );
            break;
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
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );
    // default backlog size
    lua_Integer backlog = lls_optinteger( L, 2, SOMAXCONN );
    
    // check args
    if( backlog < 1 || backlog > INT_MAX ){
        return luaL_error( L, "backlog range must be 1 to %d", INT_MAX );
    }
    // listen
    else if( listen( s->fd, (int)backlog ) != 0 ){
        // got error
        lua_pushstring( L, strerror( errno ) );
        return 1;
    }
    
    return 0;
}


static int accept_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );
    int fd = 0;
    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof( struct sockaddr_storage );

    // clear addr
    memset( (void*)&addr, 0, addrlen );

#if defined(__linux__)
    int flg = fcntl( s->fd, F_GETFL );
    
    // got error
    if( flg == -1 ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        return 2;
    }

#if defined(LINUX_ACCEPT4)
    flg = SOCK_CLOEXEC | ( ( flg & O_NONBLOCK ) ? SOCK_NONBLOCK : 0 );
    fd = accept4( s->fd, (struct sockaddr*)&addr, &addrlen, flg );
    if( fd != -1 )
    {
        lls_socket_t *cs = lua_newuserdata( L, sizeof( lls_socket_t ) );

        if( cs ){
            lstate_setmetatable( L, SOCKET_MT );
            cs->fd = fd;
            cs->addrlen = addrlen;
            // copy sockaddr
            memcpy( (void*)cs->addr, (void*)&addr, addrlen );

            return 1;
        }
        close( fd );
    }
#else
    fd = accept( s->fd, (struct sockaddr*)&addr, &addrlen );
    if( fd != -1 )
    {
        lls_socket_t *cs = lua_newuserdata( L, sizeof( lls_socket_t ) );

        if( cs &&
            fcntl( fd, F_SETFD, FD_CLOEXEC ) == 0 &&
            fcntl( fd, F_SETFL, flg ) == 0 ){
            lstate_setmetatable( L, SOCKET_MT );
            cs->fd = fd;
            cs->addrlen = addrlen;
            // copy sockaddr
            memcpy( (void*)cs->addr, (void*)&addr, addrlen );

            return 1;
        }
        close( fd );
    }
#endif


#else
    fd = accept( s->fd, (struct sockaddr*)&addr, &addrlen );
    if( fd != -1 )
    {
        lls_socket_t *cs = lua_newuserdata( L, sizeof( lls_socket_t ) );

        if( cs ){
            lstate_setmetatable( L, SOCKET_MT );
            cs->fd = fd;
            cs->addrlen = addrlen;
            // copy sockaddr
            memcpy( (void*)&cs->addr, (void*)&addr, addrlen );

            return 1;
        }

        close( fd );
    }

#endif

    // check errno
    if( errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR ||
        errno == ECONNABORTED ){
        lua_pushnil( L );
        lua_pushnil( L );
        lua_pushboolean( L, 1 );
        return 3;
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}


static int send_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );
    size_t len = 0;
    const char *buf = lls_checklstring( L, 2, &len );
    int flg = lls_optflags( L, 3 );
    ssize_t rv = 0;
    
    // invalid length
    if( !len ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( EINVAL ) );
        return 2;
    }
    
    rv = send( s->fd, buf, len, flg );
    if( rv != -1 ){
        lua_pushinteger( L, rv );
        return 1;
    }
    // again
    else if( errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR ){
        lua_pushnil( L );
        lua_pushnil( L );
        lua_pushboolean( L, 1 );
        return 3;
    }
    // closed by peer
    else if( errno == ECONNRESET ){
        return 0;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}


static int sendto_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );
    size_t len = 0;
    const char *buf = lls_checklstring( L, 2, &len );
    struct addrinfo *info = luaL_checkudata( L, 3, ADDRINFO_MT );
    int flg = lls_optflags( L, 4 );
    ssize_t rv = 0;
    
    // invalid length
    if( !len ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( EINVAL ) );
        return 2;
    }
    
    rv = sendto( s->fd, buf, len, flg, (const struct sockaddr*)info->ai_addr,
                 info->ai_addrlen );
    if( rv != -1 ){
        lua_pushinteger( L, rv );
        return 1;
    }
    // again
    else if( errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR ){
        lua_pushnil( L );
        lua_pushnil( L );
        lua_pushboolean( L, 1 );
        return 3;
    }
    // close by peer
    else if( errno == ECONNRESET ){
        return 0;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


#if defined(__linux__)
#include <sys/sendfile.h>
#define HAVE_SENDFILE_LUA   1

static int sendfile_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );
    int fd = (int)lls_checkinteger( L, 2 );
    size_t len = (size_t)lls_checkinteger( L, 3 );
    off_t offset = (off_t)lls_optinteger( L, 4, 0 );
    ssize_t rv = sendfile( s->fd, fd, &offset, len );
    
    if( rv != -1 ){
        lua_pushinteger( L, rv );
        return 1;
    }
    // again
    else if( errno == EAGAIN || errno = EINTR ){
        lua_pushnil( L );
        lua_pushboolean( L, 1 );
        return 3;
    }
    // closed by peer
    else if( errno == EPIPE ){
        return 0;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


#elif defined(__APPLE__)
#define HAVE_SENDFILE_LUA   1

static int sendfile_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );
    int fd = (int)lls_checkinteger( L, 2 );
    off_t len = (off_t)lls_checkinteger( L, 3 );
    off_t offset = (off_t)lls_optinteger( L, 4, 0 );
    
    if( sendfile( fd, s->fd, offset, &len, NULL, 0 ) == 0 ){
        lua_pushinteger( L, len );
        return 1;
    }
    // again
    else if( errno == EAGAIN || errno == EINTR ){
        lua_pushinteger( L, len );
        lua_pushnil( L );
        lua_pushboolean( L, 1 );
        return 3;
    }
    // closed by peer
    else if( errno == EPIPE ){
        return 0;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


#elif defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#define HAVE_SENDFILE_LUA   1

static int sendfile_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );
    int fd = (int)lls_checkinteger( L, 2 );
    size_t len = (size_t)lls_checkinteger( L, 3 );
    off_t offset = (off_t)lls_optinteger( L, 4, 0 );
    off_t nbytes = 0;
    
    if( sendfile( fd, s->fd, offset, len, NULL, &nbytes, 0 ) == 0 ){
        lua_pushinteger( L, nbytes );
        return 1;
    }
    // again
    else if( errno == EAGAIN || errno == EINTR ){
        lua_pushinteger( L, nbytes );
        lua_pushnil( L );
        lua_pushboolean( L, 1 );
        return 3;
    }
    // closed by peer
    else if( errno == EPIPE ){
        return 0;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}

#endif


static int recv_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );
    lua_Integer len = lls_optinteger( L, 2, DEFAULT_RECVSIZE );
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
    
    rv = recv( s->fd, buf, (size_t)len, flg );
    switch( rv ){
        // close by peer
        case 0:
        break;
        
        // got error
        case -1:
            lua_pushnil( L );
            // again
            if( errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR ){
                lua_pushnil( L );
                lua_pushboolean( L, 1 );
                rv = 3;
            }
            // got error
            else {
                lua_pushstring( L, strerror( errno ) );
                rv = 2;
            }
        break;
        
        default:
            lua_pushlstring( L, buf, rv );
            rv = 1;
    }
    
    pdealloc( buf );
    
    return rv;
}


static int recvfrom_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );
    lua_Integer len = lls_optinteger( L, 2, DEFAULT_RECVSIZE );
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
    
    rv = recvfrom( s->fd, buf, (size_t)len, flg, (struct sockaddr*)&src, &slen );
    switch( rv ){
        // close by peer
        case 0:
        break;
        
        // got error
        case -1:
            lua_pushnil( L );
            lua_pushnil( L );
            // again
            if( errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR ){
                lua_pushnil( L );
                lua_pushboolean( L, 1 );
                rv = 4;
            }
            // got error
            else {
                lua_pushstring( L, strerror( errno ) );
                rv = 3;
            }
        break;
        
        default:
            lua_pushlstring( L, buf, rv );
            // no addrinfo
            if( slen == 0 ){
                rv = 1;
            }
            else
            {
                struct addrinfo wrap = {
                    .ai_flags = 0,
                    .ai_family = s->family,
                    .ai_socktype = s->socktype,
                    .ai_protocol = s->protocol,
                    .ai_addrlen = slen,
                    .ai_addr = (struct sockaddr*)&src,
                    .ai_canonname = NULL,
                    .ai_next = NULL
                };

                // push llsocket.addr udata
                if( lls_addrinfo_alloc( L, &wrap ) ){
                    rv = 2;
                }
                // alloc error
                else {
                    lua_pop( L, 1 );
                    lua_pushnil( L );
                    lua_pushnil( L );
                    lua_pushstring( L, strerror( errno ) );
                    rv = 3;
                }
            }
    }
    
    pdealloc( buf );
    
    return rv;
}


static int connect_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );
    struct sockaddr *addr = (struct sockaddr*)&s->addr;
    socklen_t addrlen = s->addrlen;

    // check argument
    if( lua_gettop( L ) > 1 ){
        struct addrinfo *info = luaL_checkudata( L, 2, ADDRINFO_MT );
        addr = info->ai_addr;
        addrlen = info->ai_addrlen;
    }

    if( connect( s->fd, addr, addrlen ) == 0 ||
        // nonblocking connect
        errno == EINPROGRESS ){
        return 0;
    }

    // got error
    lua_pushstring( L, strerror( errno ) );

    return 1;
}


static int bind_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );

    if( bind( s->fd, (struct sockaddr*)&s->addr, s->addrlen ) == 0 ){
        return 0;
    }

    // got error
    lua_pushstring( L, strerror( errno ) );

    return 1;
}


static int protocol_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );

    return s->protocol;
}


static int socktype_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );

    return s->socktype;
}


static int family_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );

    return s->family;
}


static int fd_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );

    lua_pushinteger( L, s->fd );

    return 1;
}


static int tostring_lua( lua_State *L )
{
    lua_pushfstring( L, SOCKET_MT ": %p", lua_touserdata( L, 1 ) );
    return 1;
}


static int gc_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );

    if( s->fd != -1 ){
        close( s->fd );
    }

    return 0;
}


static int dup_lua( lua_State *L )
{
    lls_socket_t *s = luaL_checkudata( L, 1, SOCKET_MT );
    struct addrinfo info = {
        .ai_family = s->family,
        .ai_socktype = s->socktype,
        .ai_protocol = s->protocol,
        .ai_addrlen = s->addrlen,
        .ai_addr = (struct sockaddr*)&s->addr,
    };
    struct addrinfo *ptr = &info;
    int fd = 0;

    // check argument
    if( lua_gettop( L ) > 1 ){
        ptr = luaL_checkudata( L, 2, ADDRINFO_MT );
    }

    if( ( fd = dup( s->fd ) ) != -1 )
    {
        lls_socket_t *sd = NULL;

        if( fcntl( fd, F_SETFD, FD_CLOEXEC ) != -1 &&
            ( sd = lua_newuserdata( L, sizeof( lls_socket_t ) ) ) ){
            lstate_setmetatable( L, SOCKET_MT );
            *sd = (lls_socket_t){
                .fd = fd,
                .family = ptr->ai_family,
                .socktype = ptr->ai_socktype,
                .protocol = ptr->ai_protocol,
                .addrlen = ptr->ai_addrlen
            };
            // copy sockaddr
            memcpy( (void*)&sd->addr, (void*)&ptr->ai_addr, ptr->ai_addrlen );
            return 1;
        }

        // got error
        close( fd );
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}


static int new_lua( lua_State *L )
{
    struct addrinfo *info = luaL_checkudata( L, 1, ADDRINFO_MT );
#if defined(LINUX_SOCKEXT)
    int nonblock = lls_optboolean( L, 2, SOCK_NONBLOCK );
#else
    int nonblock = lls_optboolean( L, 2, 0 );
#endif
    int fd = -1;


    // create socket
#if defined(LINUX_SOCKEXT)
    fd = socket( info->ai_family,
                 info->ai_socktype|SOCK_CLOEXEC|nonblock,
                 info->ai_protocol );

#else
    fd = socket( info->ai_family, info->ai_socktype, info->ai_protocol );

#endif

    if( fd != -1 )
    {
        lls_socket_t *s = NULL;

#if !defined(LINUX_SOCKEXT)
        int fl = 0;

        if( fcntl( fd, F_SETFD, FD_CLOEXEC ) == -1 ||
            ( nonblock && ( ( fl = fcntl( fd, F_GETFL ) ) == -1 ||
              fcntl( fd, F_SETFL, fl|O_NONBLOCK ) == -1 ) ) ){
            goto FAILED;
        }
#endif

        lua_settop( L, 1 );
        if( ( s = lua_newuserdata( L, sizeof( lls_socket_t ) ) ) ){
            lstate_setmetatable( L, SOCKET_MT );
            *s = (lls_socket_t){
                .fd = fd,
                .family = info->ai_family,
                .socktype = info->ai_socktype,
                .protocol = info->ai_protocol,
                .addrlen = info->ai_addrlen
            };
            // copy sockaddr
            memcpy( (void*)&s->addr, (void*)info->ai_addr, info->ai_addrlen );

            return 1;
        }

FAILED:
        // got error
        close( fd );
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


static int pair_lua( lua_State *L )
{
    int socktype = (int)lls_checkinteger( L, 1 );
    int nonblock = lls_optboolean( L, 2, 0 );
    int protocol = (int)lls_optinteger( L, 3, 0 );
    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof( struct sockaddr_storage );
    int fds[2];


    memset( (void*)&addr, 0, addrlen );

    if( socketpair( AF_UNIX, socktype, protocol, fds ) == 0 )
    {
        lls_socket_t *s = NULL;
        int i = 0;

        lua_createtable( L, 2, 0 );
        for(; i < 2; i++ )
        {
            // set flags
            if( getsockname( fds[i], (void*)&addr, &addrlen ) == 0 &&
                fcntl( fds[i], F_SETFD, FD_CLOEXEC ) != -1 &&
                ( !nonblock || fcntl( fds[i], F_SETFL, O_NONBLOCK ) != -1 ) &&
                ( s = lua_newuserdata( L, sizeof( lls_socket_t ) ) ) ){
                *s = (lls_socket_t){
                    .fd = fds[i],
                    .family = AF_UNIX,
                    .socktype = socktype,
                    .protocol = protocol,
                    .addrlen = addrlen
                };
                // copy sockaddr
                memcpy( (void*)&s->addr, (void*)&addr, addrlen );
                lstate_setmetatable( L, SOCKET_MT );
                lua_rawseti( L, -2, i + 1 );

                continue;
            }

            close( fds[0] );
            close( fds[1] );
            goto FAILED;
        }

        return 1;
    }

FAILED:
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}



LUALIB_API int luaopen_llsocket_socket( lua_State *L )
{
    struct luaL_Reg mmethod[] = {
        { "__gc", gc_lua },
        { "__tostring", tostring_lua },
        { NULL, NULL }
    };
    struct luaL_Reg method[] = {
        { "dup", dup_lua },
        { "fd", fd_lua },
        { "family", family_lua },
        { "socktype", socktype_lua },
        { "protocol", protocol_lua },
        { "bind", bind_lua },
        { "connect", connect_lua },
        { "shutdown", shutdown_lua },
        { "close", close_lua },
        { "listen", listen_lua },
        { "accept", accept_lua },
        { "send", send_lua },
        { "sendto", sendto_lua },
#if HAVE_SENDFILE_LUA
        { "sendfile", sendfile_lua },
#else
#warning "sendfile does not implmeneted in this platform."

#endif
        { "recv", recv_lua },
        { "recvfrom", recvfrom_lua },

        // state
        { "atmark", atmark_lua },

        // address info
        { "getsockname", getsockname_lua },
        { "getpeername", getpeername_lua },

        // fd option
        { "cloexec", cloexec_lua },
        { "nonblock", nonblock_lua },

        // read-only socket option
        { "error", error_lua },
        { "acceptconn", acceptconn_lua },
        // socket option
        { "tcpnodelay", tcpnodelay_lua },
        { "tcpkeepintvl", tcpkeepintvl_lua },
        { "tcpkeepcnt", tcpkeepcnt_lua },

#if defined(HAVE_TCP_KEEPALIVE)
        { "tcpkeepalive", tcpkeepalive_lua },
#else
#warning "tcpkeepalive does not implemented in this platform."
#endif

#if defined(HAVE_TCP_CORK)
        { "tcpcork", tcpcork_lua },
#else
#warning "tcpcork does not implmeneted in this platform."

#endif

#if defined(HAVE_SO_REUSEPORT)
        { "reuseport", reuseport_lua },
#else
#warning "reuseport does not implmeneted in this platform."

#endif
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
    struct luaL_Reg *ptr = mmethod;

    // create metatable
    luaL_newmetatable( L, SOCKET_MT );
    // lock metatable
    lstate_num2tbl( L, "__metatable", 1 );
    // metamethods
    do {
        lstate_fn2tbl( L, ptr->name, ptr->func );
        ptr++;
    } while( ptr->name );
    // methods
    lua_pushstring( L, "__index" );
    lua_newtable( L );
    ptr = method;
    do {
        lstate_fn2tbl( L, ptr->name, ptr->func );
        ptr++;
    } while( ptr->name );
    lua_rawset( L, -3 );
    lua_pop( L, 1 );

    // create table
    lua_newtable( L );
    // method
    lstate_fn2tbl( L, "new", new_lua );
    lstate_fn2tbl( L, "pair", pair_lua );

    
    return 1;
}


