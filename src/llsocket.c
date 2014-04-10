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


// method
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
    lua_pushinteger( L, errno );
    
    return 2;
}


static int shutdown_lua( lua_State *L )
{
    int fd = luaL_checkint( L, 1 );
    int how = luaL_checkint( L, 2 );
    
    if( shutdown( fd, how ) == 0 ){
        lua_pushboolean( L, 1 );
        return 1;
    }
    
    // got error
    lua_pushboolean( L, 0 );
    lua_pushinteger( L, errno );
    
    return 2;
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
            lua_pushnil( L );
            lua_pushinteger( L, errno );
            return 2;
        }
    }
    
    // success
    lua_pushboolean( L, 1 );
    
    return 1;
}


static int listen_lua( lua_State *L )
{
    int fd = luaL_checkint( L, 1 );
    // default backlog size
    lua_Integer backlog = SOMAXCONN;
    
    // check args
    if( !lua_isnoneornil( L, 2 ) )
    {
        backlog = luaL_checkinteger( L, 2 );
        if( backlog < 1 || backlog > INT_MAX ){
            return luaL_error( L, "backlog range must be 1 to %d", INT_MAX );
        }
    }
    
    // listen
    if( listen( fd, (int)backlog ) == 0 ){
        lua_pushboolean( L, 1 );
        return 1;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushinteger( L, errno );
    
    return 2;
}


static int accept_lua( lua_State *L )
{
    int fd = luaL_checkint( L, 1 );
    int cfd = 0;
    
    // check args
    if( !lua_isnoneornil( L, 2 ) ){
        luaL_checktype( L, 2, LUA_TBOOLEAN );
    }
    
    cfd = accept( fd, NULL, NULL );
    if( cfd != -1 )
    {
        if( fcntl( cfd, F_SETFD, FD_CLOEXEC ) == 0 && 
            ( lua_toboolean( L, 2 ) ? !fcntl( cfd, F_SETFL, O_NONBLOCK ) : 1 ) ){
            lua_pushinteger( L, cfd );
            return 1;
        }
        
        close( cfd );
    }
    
    // got error
    lua_pushnil( L );
    lua_pushinteger( L, errno );
    
    return 2;
}


// fd option
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


// socket option
static int sockopt_int_lua( lua_State *L, int level, int optname, int type )
{
    int fd = luaL_checkint( L, 1 );
    
    return lls_sockopt_int_lua( L, fd, level, optname, type );
}

static int type_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_TYPE, LUA_TNUMBER );
}

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

static int error_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_ERROR, LUA_TBOOLEAN );
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



LUALIB_API int luaopen_llsocket( lua_State *L )
{
    struct luaL_Reg method[] = {
        // with socket-fd
        // method
        { "sockname", sockname_lua },
        { "shutdown", shutdown_lua },
        { "close", close_lua },
        { "listen", listen_lua },
        { "accept", accept_lua },
        { NULL, NULL }
    };
    struct luaL_Reg opt_method[] = {
        // fd option
        { "cloexec", cloexec_lua },
        { "nonblock", nonblock_lua },
        // socket option
        { "type", type_lua },
        { "nodelay", nodelay_lua },
        { "reuseaddr", reuseaddr_lua },
        { "broadcast", broadcast_lua },
        { "debug", debug_lua },
        { "error", error_lua },
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
    int i;
    
    // create table
    lua_newtable( L );
    // open libs
    lua_pushstring( L, "inet" );
    luaopen_llsocket_inet( L );
    lua_rawset( L, -3 );
    lua_pushstring( L, "unix" );
    luaopen_llsocket_unix( L );
    lua_rawset( L, -3 );

    // method
    i = 0;
    while( method[i].name ){
        lstate_fn2tbl( L, method[i].name, method[i].func );
        i++;
    }
    // option method
    lua_pushstring( L, "opt" );
    lua_newtable( L );
    i = 0;
    while( opt_method[i].name ){
        lstate_fn2tbl( L, opt_method[i].name, opt_method[i].func );
        i++;
    }
    // constants
    // for connect and bind
    lstate_num2tbl( L, "SOCK_STREAM", SOCK_STREAM );
    lstate_num2tbl( L, "SOCK_DGRAM", SOCK_DGRAM );
    lstate_num2tbl( L, "SOCK_SEQPACKET", SOCK_SEQPACKET );
    // for shutdown
    lstate_num2tbl( L, "SHUT_RD", SHUT_RD );
    lstate_num2tbl( L, "SHUT_WR", SHUT_WR );
    lstate_num2tbl( L, "SHUT_RDWR", SHUT_RDWR );
    lua_rawset( L, -3 );
    
    return 1;
}


