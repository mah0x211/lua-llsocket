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


static int getnameinfo_lua( lua_State *L )
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


static int addr_lua( lua_State *L )
{
    struct addrinfo *info = luaL_checkudata( L, 1, ADDRINFO_MT );
    struct sockaddr_un *uaddr = NULL;
    struct sockaddr_in *iaddr = NULL;
    struct sockaddr_in6 *iaddr6 = NULL;
    char buf[INET6_ADDRSTRLEN] = { 0 };

    switch( info->ai_family ){
        case AF_INET:
            lua_createtable( L, 0, 2 );
            iaddr = (struct sockaddr_in*)info->ai_addr;
            lstate_num2tbl( L, "port", ntohs( iaddr->sin_port ) );
            lstate_str2tbl( L, "ip", inet_ntop( info->ai_family,
                                                (const void*)&iaddr->sin_addr,
                                                buf, INET6_ADDRSTRLEN ) );
            return 1;

        case AF_INET6:
            lua_createtable( L, 0, 2 );
            iaddr6 = (struct sockaddr_in6*)info->ai_addr;
            lstate_num2tbl( L, "port", ntohs( iaddr->sin_port ) );
            lstate_str2tbl( L, "ip", inet_ntop( info->ai_family,
                                                (const void*)&iaddr6->sin6_addr,
                                                buf, INET6_ADDRSTRLEN ) );
            return 1;

        case AF_UNIX:
            lua_createtable( L, 0, 1 );
            uaddr = (struct sockaddr_un*)info->ai_addr;
            lstate_str2tbl( L, "path", uaddr->sun_path );
            return 1;

        // unsupported family
        default:
            return 0;
    }
}


static int canonname_lua( lua_State *L )
{
    struct addrinfo *info = luaL_checkudata( L, 1, ADDRINFO_MT );

    if( info->ai_canonname ){
        lua_pushstring( L, info->ai_canonname );
        return 1;
    }

    return 0;
}


static int protocol_lua( lua_State *L )
{
    struct addrinfo *info = luaL_checkudata( L, 1, ADDRINFO_MT );

    lua_pushinteger( L, info->ai_protocol );

    return 1;
}


static int socktype_lua( lua_State *L )
{
    struct addrinfo *info = luaL_checkudata( L, 1, ADDRINFO_MT );

    lua_pushinteger( L, info->ai_socktype );

    return 1;
}


static int family_lua( lua_State *L )
{
    struct addrinfo *info = luaL_checkudata( L, 1, ADDRINFO_MT );

    lua_pushinteger( L, info->ai_family );

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
        { "family", family_lua },
        { "socktype", socktype_lua },
        { "protocol", protocol_lua },
        { "cannoname", canonname_lua },
        { "addr", addr_lua },
        { "getnameinfo", getnameinfo_lua },
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


