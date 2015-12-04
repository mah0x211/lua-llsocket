/*
 *  Copyright 2015 Masatoshi Teruya. All rights reserved.
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
 *  opt.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 15/08/12.
 *
 */

#include "llsocket.h"


// MARK: fd option
#define fcntl_lua(L,getfl,setfl,fl) ({ \
    int fd = (int)luaL_checkinteger( L, 1 ); \
    lls_fcntl_lua( L, fd, getfl, setfl, fl ); \
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
    int fd = (int)luaL_checkinteger( L, 1 ); \
    lls_sockopt_int_lua( L, fd, level, optname, type ); \
})


// readonly
static int type_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_TYPE, LUA_TNUMBER );
}

static int error_lua( lua_State *L )
{
    return sockopt_int_lua( L, SOL_SOCKET, SO_ERROR, LUA_TNUMBER );
}


// writable
static int tcpnodelay_lua( lua_State *L )
{
    return sockopt_int_lua( L, IPPROTO_TCP, TCP_NODELAY, LUA_TBOOLEAN );
}


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
    int fd = (int)luaL_checkinteger( L, 1 ); \
    lls_sockopt_timeval_lua( L, fd, level, opt ); \
})

static int rcvtimeo_lua( lua_State *L )
{
    return sockopt_timeval_lua( L, SOL_SOCKET, SO_RCVTIMEO );
}

static int sndtimeo_lua( lua_State *L )
{
    return sockopt_timeval_lua( L, SOL_SOCKET, SO_SNDTIMEO );
}


LUALIB_API int luaopen_llsocket_opt( lua_State *L )
{
    struct luaL_Reg method[] = {
        // fd option
        { "cloexec", cloexec_lua },
        { "nonblock", nonblock_lua },
        // read-only socket option
        { "type", type_lua },
        { "error", error_lua },
        // socket option
        { "tcpnodelay", tcpnodelay_lua },
#if defined(HAVE_TCP_CORK)
        { "tcpcork", tcpcork_lua },
#else
#warning "tcpcork does not implmeneted in this platform."

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
    struct luaL_Reg *ptr = method;
    
    lua_newtable( L );
    do {
        lstate_fn2tbl( L, ptr->name, ptr->func );
        ptr++;
    } while( ptr->name );
    
    return 1;
}


