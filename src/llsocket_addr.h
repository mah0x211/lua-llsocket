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
 *  llsocket_addr.h
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 14/12/12.
 *  Copyright 2014 Masatoshi Teruya. All rights reserved.
 *
 */


#ifndef ___LLSOCKET_ADDR_LUA___
#define ___LLSOCKET_ADDR_LUA___

#include "llsocket.h"

#define LLS_ADDR_MT   "llsocket.addr"

typedef struct {
    struct sockaddr_storage addr;
    socklen_t len;
} lls_addr_t;


LUALIB_API int luaopen_llsocket_addr( lua_State *L );


static inline int llsocket_addr_alloc( lua_State *L, 
                                       struct sockaddr_storage *addr, 
                                       socklen_t len )
{
    lls_addr_t *llsa = lua_newuserdata( L, sizeof( lls_addr_t ) );
    
    // mem-error
    if( !llsa ){
        return -1;
    }
    
    memcpy( (void*)&llsa->addr, (void*)addr, len );
    llsa->len = len;
    // set metatable
    luaL_getmetatable( L, LLS_ADDR_MT );
    lua_setmetatable( L, -2 );
    
    return 0;
}


#endif
