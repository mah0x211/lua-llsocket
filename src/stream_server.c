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
 *  stream_server.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 14/03/29.
 *  Copyright 2014 Masatoshi Teruya. All rights reserved.
 *
 */

#include "lls_inet.h"
#include "lls_stream.h"

#define MODULE_MT   "llsocket.inet.stream.server"


static int listen_lua( lua_State *L )
{
    return lls_stream_listen( L, MODULE_MT );
}


static int islisten_lua( lua_State *L )
{
    return lls_stream_islisten( L, MODULE_MT );
}


static int fd_lua( lua_State *L )
{
    return lls_fd( L, MODULE_MT );
}

static int close_lua( lua_State *L )
{
    return lls_close( L, MODULE_MT );
}

static int nonblock_lua( lua_State *L )
{
    return lls_nonblock( L, MODULE_MT );
}

static int nodelay_lua( lua_State *L )
{
    return lls_inet_stream_nodelay( L, MODULE_MT );
}

static int bind_lua( lua_State *L )
{
    return lls_bind( L, MODULE_MT );
}

/* metamethods */
static int tostring_lua( lua_State *L )
{
    return lls_tostring( L, MODULE_MT );
}

static int alloc_lua( lua_State *L )
{
    return lls_inet_alloc( L, MODULE_MT, AI_PASSIVE, SOCK_STREAM );
}


LUALIB_API int luaopen_llsocket_inet_stream_server( lua_State *L )
{
    struct luaL_Reg mmethod[] = {
        { "__gc", lls_inet_gc },
        { "__tostring", tostring_lua },
        { NULL, NULL }
    };
    struct luaL_Reg method[] = {
        { "fd", fd_lua },
        { "close", close_lua },
        { "nonblock", nonblock_lua },
        { "nodelay", nodelay_lua },
        { "bind", bind_lua },
        { "listen", listen_lua },
        { "isListen", islisten_lua },
        { NULL, NULL }
    };
    
    lls_define_mt( L, MODULE_MT, mmethod, method );
    lua_pop( L, 1 );
    lua_pushcfunction( L, alloc_lua );
    
    return 1;
}

