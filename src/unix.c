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
 *  unix.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 14/04/10.
 *  Copyright 2014 Masatoshi Teruya. All rights reserved.
 *
 */

#include "llsocket.h"

// unix-domain socket max path length
#define UNIXPATH_MAX    (sizeof(((struct sockaddr_un*)0)->sun_path))


typedef int(*connbind_t)( int, const struct sockaddr*, socklen_t );

static int connbind_lua( lua_State *L, connbind_t proc )
{
    size_t len = 0;
    const char *path = luaL_checklstring( L, 1, &len );
    int socktype = luaL_checkint( L, 2 );
    int nonblock = 0;
    int fd = 0;
    
    
    // check arguments
    
    if( !lua_isnoneornil( L, 3 ) ){
        luaL_checktype( L, 3, LUA_TBOOLEAN );
        nonblock = lua_toboolean( L, 3 );
    }
    // length too large
    if( len > UNIXPATH_MAX ){
        errno = ENAMETOOLONG;
    }
    // create socket descriptor
    else if( ( fd = socket( AF_UNIX, socktype, 0 ) ) != -1 )
    {
        struct sockaddr_un addr;
        
        addr.sun_family = AF_UNIX,
        memcpy( (void*)&addr.sun_path, (void*)path, len );
        addr.sun_path[len] = 0;
        len = sizeof( struct sockaddr_un );
        
        // set flags
        fcntl( fd, F_SETFD, FD_CLOEXEC );
        if( nonblock ){
            int fl = fcntl( fd, F_GETFL );
            fcntl( fd, F_SETFL, fl|O_NONBLOCK );
        }
        
        if( proc( fd, (struct sockaddr*)&addr, (socklen_t)len ) == 0 || 
            // nonblocking connect
            ( proc == connect && errno == EINPROGRESS ) ){
            lua_pushinteger( L, fd );
            return 1;
        }
        
        close( fd );
    }
    
    // got error
    lua_pushnil( L );
    lua_pushinteger( L, errno );
    
    return 2;
}


// method
static int connect_lua( lua_State *L )
{
    return connbind_lua( L, connect );
}

static int bind_lua( lua_State *L )
{
    return connbind_lua( L, bind );
}


LUALIB_API int luaopen_llsocket_unix( lua_State *L )
{
    struct luaL_Reg method[] = {
        // create socket-fd
        { "connect", connect_lua },
        { "bind", bind_lua },
        { NULL, NULL }
    };
    int i;
    
    // method
    lua_newtable( L );
    i = 0;
    while( method[i].name ){
        lstate_fn2tbl( L, method[i].name, method[i].func );
        i++;
    }
    
    return 1;
}
