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
 *  inet.c
 *  lua-socket
 *
 *  Created by Masatoshi Teruya on 14/03/29.
 *  Copyright 2014 Masatoshi Teruya. All rights reserved.
 *
 */

#include "llsocket.h"


static int inet_lua( lua_State *L, const char *mt, int flags, int socktype )
{
    // afd_sock_t *as, int type, const char *addr, size_t len )
    size_t hlen, plen;
    const char *host = lua_tolstring( L, 1, &hlen );
    const char *port = lua_tolstring( L, 2, &plen );
    llsocket_t *s = lua_newuserdata( L, sizeof( llsocket_t ) );
    
    // host and port undefined
    if( !hlen && !plen ){
        return luaL_error( L, "does not specified host or port" );
    }
    else if( lls_inet_init( s, host, hlen, port, plen, flags, socktype ) == 0 ){
        lstate_setmetatable( L, mt );
        return 1;
    }
    // got error
    lua_pushnil( L );
    lua_pushinteger( L, errno );

    return 2;
}


static int server_stream_lua( lua_State *L )
{
    return inet_lua( L, LLS_SERVER, AI_PASSIVE, SOCK_STREAM );
}


static int client_stream_lua( lua_State *L )
{
    return inet_lua( L, LLS_CLIENT, 0, SOCK_STREAM );
}


static int server_dgram_lua( lua_State *L )
{
    return inet_lua( L, LLS_SERVER, AI_PASSIVE, SOCK_DGRAM );
}


static int client_dgram_lua( lua_State *L )
{
    return inet_lua( L, LLS_CLIENT, 0, SOCK_DGRAM );
}


LUALIB_API int luaopen_llsocket_inet( lua_State *L )
{
    // define metatable
    lls_server_mt( L );
    lua_pop( L, 1 );
    lls_client_mt( L );
    lua_pop( L, 1 );
    
    // add methods
    lua_newtable( L );
    // stream
    lua_pushstring( L, "stream" );
    lua_newtable( L );
    lstate_fn2tbl( L, "server", server_stream_lua );
    lstate_fn2tbl( L, "client", client_stream_lua );
    lua_rawset( L, -3 );
    // dgram
    lua_pushstring( L, "dgram" );
    lua_newtable( L );
    lstate_fn2tbl( L, "server", server_dgram_lua );
    lstate_fn2tbl( L, "client", client_dgram_lua );
    lua_rawset( L, -3 );

    return 1;
}
