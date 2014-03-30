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
 *  server.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 14/03/29.
 *  Copyright 2014 Masatoshi Teruya. All rights reserved.
 *
 */

#include "llsocket.h"


static int bind_lua( lua_State *L )
{
    llsocket_t *s = luaL_checkudata( L, 1, LLS_SERVER );
    int backlog = (int)lua_tointeger( L, 2 );
    
    // use default backlog size
    if( !backlog ){
        backlog = SOMAXCONN;
    }
    
    // bind and listen
    if( bind( s->fd, (struct sockaddr*)s->addr, (socklen_t)s->addrlen ) == 0 && 
        ( s->type == SOCK_DGRAM || listen( s->fd, backlog ) == 0 ) ){
        lua_pushboolean( L, 1 );
        return 1;
    }
    
    // got error
    lua_pushboolean( L, 0 );
    lua_pushinteger( L, errno );
    
    return 2;
}

static int fd_lua( lua_State *L )
{
    return fd_mt( L, LLS_CLIENT );
}


/* metamethods */
static int gc_lua( lua_State *L )
{
    return gc_mt( L );
}

static int tostring_lua( lua_State *L )
{
    tostring_mt( L, LLS_SERVER );
    return 1;
}


int lls_server_mt( lua_State *L )
{
    struct luaL_Reg mmethod[] = {
        { "__gc", gc_lua },
        { "__tostring", tostring_lua },
        { NULL, NULL }
    };
    struct luaL_Reg method[] = {
        { "fd", fd_lua },
        { "close", close_lua },
        { "bind", bind_lua },
        { NULL, NULL }
    };
    int i;
    
    // create table __metatable
    luaL_newmetatable( L, LLS_SERVER );
    // metamethods
    i = 0;
    while( mmethod[i].name ){
        lstate_fn2tbl( L, mmethod[i].name, mmethod[i].func );
        i++;
    }
    // methods
    lua_pushstring( L, "__index" );
    lua_newtable( L );
    i = 0;
    while( method[i].name ){
        lstate_fn2tbl( L, method[i].name, method[i].func );
        i++;
    }
    lua_rawset( L, -3 );
    
    return 1;
}


