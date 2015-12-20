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
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 14/03/29.
 *  Copyright 2014 Masatoshi Teruya. All rights reserved.
 *
 */

#include "llsocket.h"


static int getaddrinfo_lua( lua_State *L, int ai_family )
{
    const char *node = lls_optstring( L, 1, NULL );
    const char *service = lls_optstring( L, 2, NULL );
    struct addrinfo hints = {
        // AF_INET:ipv4 | AF_INET6:ipv6
        .ai_family = ai_family,
        // SOCK_STREAM:tcp | SOCK_DGRAM:udp | SOCK_SEQPACKET
        .ai_socktype = (int)lls_optinteger( L, 3, 0 ),
        // IPPROTO_TCP:tcp | IPPROTO_UDP:udp | 0:automatic
        .ai_protocol = (int)lls_optinteger( L, 4, 0 ),
        // AI_PASSIVE:bind socket if node is null
        .ai_flags = lls_optflags( L, 5 ),
        // initialize
        .ai_addrlen = 0,
        .ai_addr = NULL,
        .ai_canonname = NULL,
        .ai_next = NULL
    };
    struct addrinfo *list = NULL;
    struct addrinfo *ptr = NULL;
    int idx = 1;
    int rc = 0;

    // getaddrinfo is better than inet_pton.
    // i wonder that can be ignore an overhead of creating socket
    // descriptor when i simply want to confirm correct address?
    // wildcard ip-address
    if( ( rc = getaddrinfo( node, service, &hints, &list ) ) != 0 ){
        lua_pushnil( L );
        lua_pushstring( L, gai_strerror( rc ) );
        return 2;
    }

    // create address table
    rc = 1;
    lua_newtable( L );
    ptr = list;
    while( ptr )
    {
        if( lls_addrinfo_alloc( L, ptr ) ){
            lua_rawseti( L, -2, idx++ );
            // check next
            ptr = ptr->ai_next;
            continue;
        }

        // mem-error
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        rc = 2;
        break;
    }

    // remove address-list
    freeaddrinfo( list );

    return rc;
}


static int getaddrinfo4_lua( lua_State *L )
{
    return getaddrinfo_lua( L, AF_INET );
}


static int getaddrinfo6_lua( lua_State *L )
{
    return getaddrinfo_lua( L, AF_INET6 );
}


LUALIB_API int luaopen_llsocket_inet( lua_State *L )
{
    lua_newtable( L );
    lstate_fn2tbl( L, "getaddrinfo", getaddrinfo4_lua );
    lstate_fn2tbl( L, "getaddrinfo6", getaddrinfo6_lua );

    return 1;
}

