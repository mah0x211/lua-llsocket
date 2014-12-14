/*
 *  Copyright 2014 Masatoshi Teruya. All rights reserved.
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
 *  addr.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 14/12/12.
 *
 */

#include "llsocket_addr.h"

#define MODULE_MT   LLS_ADDR_MT

static int info_lua( lua_State *L )
{
    lls_addr_t *llsa = luaL_checkudata( L, 1, MODULE_MT );
    int flag = 0;
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    int rc = getnameinfo( (struct sockaddr*)&llsa->addr, llsa->len, host, 
                          NI_MAXHOST, serv, NI_MAXSERV, flag );
    
    if( rc == 0 ){
        lua_createtable( L, 0, 2 );
        lstate_str2tbl( L, "host", host );
        lstate_str2tbl( L, "service", serv );
        return 1;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, gai_strerror( rc ) );
    
    return 2;
}


static int tostring_lua( lua_State *L )
{
    lua_pushfstring( L, MODULE_MT ": %p", lua_touserdata( L, 1 ) );
    return 1;
}


LUALIB_API int luaopen_llsocket_addr( lua_State *L )
{
    struct luaL_Reg mmethod[] = {
        { "__tostring", tostring_lua },
        { NULL, NULL }
    };
    struct luaL_Reg method[] = {
        { "info", info_lua },
        { NULL, NULL }
    };
    struct luaL_Reg *ptr = mmethod;
    
    luaL_newmetatable( L, MODULE_MT );
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
    
    return 0;
}


