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
 *  addrinfo.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 15/12/14.
 *
 */


#include "llsocket.h"


static int nameinfo_lua( lua_State *L )
{
    struct addrinfo *info = luaL_checkudata( L, 1, ADDRINFO_MT );
    int flag = lls_optflags( L, 2 );
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    int rc = getnameinfo( info->ai_addr, info->ai_addrlen, host, NI_MAXHOST,
                          serv, NI_MAXSERV, flag );

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


static int info_lua( lua_State *L )
{
    struct addrinfo *info = luaL_checkudata( L, 1, ADDRINFO_MT );
    struct sockaddr_un *uaddr = NULL;
    struct sockaddr_in *iaddr = NULL;

    // struct addrinfo
    if( info->ai_canonname ){
        lua_createtable( L, 0, 5 );
        lstate_str2tbl( L, "canonname", info->ai_canonname );
    }
    else {
        lua_createtable( L, 0, 4 );
    }
    lstate_num2tbl( L, "family", info->ai_family );
    lstate_num2tbl( L, "socktype", info->ai_socktype );
    lstate_num2tbl( L, "protocol", info->ai_protocol );
    // struct sockaddr
    lua_pushstring( L, "addr" );
    switch( info->ai_family ){
        case AF_INET:
            lua_createtable( L, 0, 2 );
            iaddr = (struct sockaddr_in*)&info->ai_addr;
            lstate_num2tbl( L, "port", ntohs( iaddr->sin_port ) );
            lstate_str2tbl( L, "ip", inet_ntoa( iaddr->sin_addr ) );
            lua_rawset( L, -3 );
        break;

        case AF_UNIX:
            lua_createtable( L, 0, 1 );
            uaddr = (struct sockaddr_un*)info->ai_addr;
            lstate_str2tbl( L, "path", uaddr->sun_path );
            lua_rawset( L, -3 );
        break;

        // unsupported family
        default:
            lua_pop( L, 1 );
    }

    return 1;
}


static int tostring_lua( lua_State *L )
{
    lua_pushfstring( L, ADDRINFO_MT ": %p", lua_touserdata( L, 1 ) );
    return 1;
}


static int gc_lua( lua_State *L )
{
    struct addrinfo *info = luaL_checkudata( L, 1, ADDRINFO_MT );

    if( info->ai_canonname ){
        pdealloc( info->ai_canonname );
    }
    pdealloc( info->ai_addr );

    return 0;
}


LUALIB_API int luaopen_llsocket_addrinfo( lua_State *L )
{
    struct luaL_Reg mmethod[] = {
        { "__gc", gc_lua },
        { "__tostring", tostring_lua },
        { NULL, NULL }
    };
    struct luaL_Reg method[] = {
        { "info", info_lua },
        { "nameinfo", nameinfo_lua },
        { NULL, NULL }
    };
    struct luaL_Reg *ptr = mmethod;
    
    luaL_newmetatable( L, ADDRINFO_MT );
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


