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
 *  lls_stream.h
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 14/04/03.
 *  Copyright 2014 Masatoshi Teruya. All rights reserved.
 *
 */

#ifndef ___LLSOCKET_STREAM_LUA___
#define ___LLSOCKET_STREAM_LUA___

#include "llsocket.h"

LUALIB_API int luaopen_llsocket_inet_stream( lua_State *L );
LUALIB_API int luaopen_llsocket_inet_stream_server( lua_State *L );
LUALIB_API int luaopen_llsocket_inet_stream_client( lua_State *L );

static inline int lls_stream_delay( lua_State *L, const char *tname )
{
    llsocket_t *s = luaL_checkudata( L, 1, tname );
    int flg = 0;
    
    // check args
    luaL_checktype( L, 2, LUA_TBOOLEAN );
    flg = lua_toboolean( L, 2 );
    
    // set delay flag
    if( setsockopt( s->fd, IPPROTO_TCP, TCP_NODELAY, &flg, sizeof(int) ) == 0 ){
        lua_pushboolean( L, 1 );
        return 1;
    }
    
    // got error
    lua_pushboolean( L, 0 );
    lua_pushinteger( L, errno );
    
    return 2;
}


static inline int lls_stream_listen( lua_State *L, const char *tname )
{
    llsocket_t *s = luaL_checkudata( L, 1, tname );
    // use default backlog size
    int backlog = SOMAXCONN;
    
    // check args
    if( !lua_isnoneornil( L, 2 ) )
    {
        backlog = luaL_checkint( L, 2 );
        if( backlog < 1 ){
            return luaL_argerror( L, 1, "backlog must be larger than 0" );
        }
    }
    
    // listen
    if( listen( s->fd, backlog ) == 0 ){
        lua_pushboolean( L, 1 );
        return 1;
    }
    
    // got error
    lua_pushboolean( L, 0 );
    lua_pushinteger( L, errno );
    
    return 2;
}


static inline int lls_stream_islisten( lua_State *L, const char *tname )
{
    llsocket_t *s = luaL_checkudata( L, 1, tname );
    socklen_t len = sizeof(int);
    int flg = 0;
    
    if( getsockopt( s->fd, SOL_SOCKET, SO_ACCEPTCONN, &flg, &len ) == 0 ){
        lua_pushboolean( L, flg );
        return 1;
    }

    // got error
    lua_pushboolean( L, 0 );
    lua_pushinteger( L, errno );
    
    return 2;
}


#endif



