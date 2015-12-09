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
#define DEFAULT_RECVSIZE    4096


// MARK: method
static int sockname_lua( lua_State *L )
{
    int fd = (int)luaL_checkinteger( L, 1 );
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
                iaddr = (struct sockaddr_in*)&addr;
                lstate_num2tbl( L, "port", ntohs( iaddr->sin_port ) );
                lstate_str2tbl( L, "addr", inet_ntoa( iaddr->sin_addr ) );
                return 1;
            
            case AF_UNIX:
                lstate_str2tbl( L, "family", "unix" );
                uaddr = (struct sockaddr_un*)&addr;
                lstate_str2tbl( L, "path", uaddr->sun_path ); 
                return 1;
            
            default:
                lua_pop( L, 1 );
                errno = ENOTSUP;
        }
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


static int peername_lua( lua_State *L )
{
    int fd = (int)luaL_checkinteger( L, 1 );
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
    int fd = (int)luaL_checkinteger( L, 1 );
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
    int fd = (int)luaL_checkinteger( L, 1 );
    int how = (int)luaL_checkinteger( L, 2 );
    
    if( shutdown( fd, how ) == 0 ){
        // got error
        lua_pushstring( L, strerror( errno ) );
        return 1;
    }
    
    return 0;
}


static int close_lua( lua_State *L )
{
    int fd = (int)luaL_checkinteger( L, 1 );
    
    if( fd )
    {
        int rc = 0;

        // check arguments
        if( !lua_isnoneornil( L, 2 ) ){
            int how = (int)luaL_checkinteger( L, 2 );
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
    int fd = (int)luaL_checkinteger( L, 1 );
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
    int fd = (int)luaL_checkinteger( L, 1 );
    int cfd = accept( fd, NULL, NULL );
    
    if( cfd != -1 )
    {
        if( fcntl( cfd, F_SETFD, FD_CLOEXEC ) != -1 ){
            lua_pushinteger( L, cfd );
            return 1;
        }
        
        close( cfd );
    }
    // check errno
    else if( errno == EAGAIN || errno == EWOULDBLOCK ||
             errno == EINTR || errno == ECONNABORTED ){
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


static int accept_inherits_lua( lua_State *L )
{
    int fd = (int)luaL_checkinteger( L, 1 );
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
        // check errno
        else if( errno == EAGAIN || errno == EWOULDBLOCK ||
                 errno == EINTR || errno == ECONNABORTED ){
            lua_pushnil( L );
            lua_pushnil( L );
            lua_pushboolean( L, 1 );
            return 3;
        }
    }

#else
    if( ( cfd = accept( fd, NULL, NULL ) ) != -1 ){
        lua_pushinteger( L, cfd );
        return 1;
    }
    // check errno
    else if( errno == EAGAIN || errno == EWOULDBLOCK ||
             errno == EINTR || errno == ECONNABORTED ){
        lua_pushnil( L );
        lua_pushnil( L );
        lua_pushboolean( L, 1 );
        return 3;
    }

#endif

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}


static int send_lua( lua_State *L )
{
    int fd = (int)luaL_checkinteger( L, 1 );
    size_t len = 0; 
    const char *buf = luaL_checklstring( L, 2, &len );
    int flg = lls_optflags( L, 3 );
    ssize_t rv = 0;
    
    // invalid length
    if( !len ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( EINVAL ) );
        return 2;
    }
    
    rv = send( fd, buf, len, flg );
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
    int fd = (int)luaL_checkinteger( L, 1 );
    size_t len = 0; 
    const char *buf = luaL_checklstring( L, 2, &len );
    lls_addr_t *llsaddr = (lls_addr_t*)lls_checkudata( L, 3, LLS_ADDR_MT );
    int flg = lls_optflags( L, 4 );
    ssize_t rv = 0;
    
    // invalid length
    if( !len ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( EINVAL ) );
        return 2;
    }
    
    rv = sendto( fd, buf, len, flg, (const struct sockaddr*)&llsaddr->addr, 
                 llsaddr->len );
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
    int fd = (int)luaL_checkinteger( L, 1 );
    int ifd = (int)luaL_checkinteger( L, 2 );
    size_t len = (size_t)luaL_checkinteger( L, 3 );
    off_t offset = (off_t)luaL_optinteger( L, 4, 0 );
    ssize_t rv = sendfile( fd, ifd, &offset, len );
    
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
    int fd = (int)luaL_checkinteger( L, 1 );
    int ifd = (int)luaL_checkinteger( L, 2 );
    off_t len = (off_t)luaL_checkinteger( L, 3 );
    off_t offset = (off_t)luaL_optinteger( L, 4, 0 );
    
    if( sendfile( ifd, fd, offset, &len, NULL, 0 ) == 0 ){
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
    int fd = (int)luaL_checkinteger( L, 1 );
    int ifd = (int)luaL_checkinteger( L, 2 );
    size_t len = (size_t)luaL_checkinteger( L, 3 );
    off_t offset = (off_t)luaL_optinteger( L, 4, 0 );
    off_t nbytes = 0;
    
    if( sendfile( ifd, fd, offset, len, NULL, &nbytes, 0 ) == 0 ){
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
    int fd = (int)luaL_checkinteger( L, 1 );
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
    
    rv = recv( fd, buf, (size_t)len, flg );
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
    int fd = (int)luaL_checkinteger( L, 1 );
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
    
    rv = recvfrom( fd, buf, (size_t)len, flg, (struct sockaddr*)&src, &slen );
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
            // push llsocket.addr udata
            else if( llsocket_addr_alloc( L, &src, slen ) == 0 ){
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
    
    pdealloc( buf );
    
    return rv;
}


LUALIB_API int luaopen_llsocket( lua_State *L )
{
    struct luaL_Reg method[] = {
        // with socket-fd
        // method
        { "sockname", sockname_lua },
        { "peername", peername_lua },
        { "atmark", atmark_lua },
        { "shutdown", shutdown_lua },
        { "close", close_lua },
        { "listen", listen_lua },
        { "accept", accept_lua },
        { "acceptInherits", accept_inherits_lua },
        { "send", send_lua },
        { "sendto", sendto_lua },
#if HAVE_SENDFILE_LUA
        { "sendfile", sendfile_lua },
#else
#warning "sendfile does not implmeneted in this platform."

#endif
        { "recv", recv_lua },
        { "recvfrom", recvfrom_lua },
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
    
    lua_pushstring( L, "device" );
    luaopen_llsocket_device( L );
    lua_rawset( L, -3 );
    
    lua_pushstring( L, "opt" );
    luaopen_llsocket_opt( L );
    lua_rawset( L, -3 );

    // no alloc interface
    luaopen_llsocket_addr( L );

    // method
    ptr = method;
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

    // for socket protocol
#define GEN_SOCKET_PROTOCOLS_DECL

    // for send/recv flags
#define GEN_SENDRECV_FLAGS_DECL
    
    
    return 1;
}


