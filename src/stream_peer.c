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
 *  stream_peer.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 14/04/04.
 *  Copyright 2014 Masatoshi Teruya. All rights reserved.
 *
 */

#include "lls_inet.h"
#include "lls_stream.h"


#define MODULE_MT   LLS_STREAM_PEER_MT

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


/* metamethods */
static int tostring_lua( lua_State *L )
{
    return lls_tostring( L, MODULE_MT );
}


LUALIB_API int luaopen_llsocket_inet_stream_peer( lua_State *L )
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
        { NULL, NULL }
    };
    
    lls_define_mt( L, MODULE_MT, mmethod, method );
    lua_pop( L, 1 );
    
    return 0;
}

