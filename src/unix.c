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


static int getaddrinfo_lua( lua_State *L )
{
    size_t len = 0;
    const char *path = lauxh_checklstring( L, 1, &len );
    struct addrinfo tmpl = {
        // AF_INET:ipv4 | AF_INET6:ipv6
        .ai_family = AF_UNIX,
        // SOCK_STREAM:tcp | SOCK_DGRAM:udp | SOCK_SEQPACKET
        .ai_socktype = (int)lauxh_optinteger( L, 2, 0 ),
        // IPPROTO_TCP:tcp | IPPROTO_UDP:udp | 0:automatic
        .ai_protocol = (int)lauxh_optinteger( L, 3, 0 ),
        // AI_PASSIVE:bind socket if node is null
        .ai_flags = lauxh_optflags( L, 4 ),
        // initialize
        .ai_addrlen = sizeof( struct sockaddr_un ),
        .ai_addr = NULL,
        .ai_canonname = NULL,
        .ai_next = NULL
    };
    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
        .sun_path = {0}
    };

    // length too large
    if( len > UNIXPATH_MAX ){
        errno = ENAMETOOLONG;
    }
    else
    {
        memcpy( (void*)&addr.sun_path, (void*)path, len );
        addr.sun_path[len] = 0;
        tmpl.ai_addr = (struct sockaddr*)&addr;
        if( lls_addrinfo_alloc( L, &tmpl ) ){
            return 1;
        }
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}


LUALIB_API int luaopen_llsocket_unix( lua_State *L )
{
    // add functions
    lua_newtable( L );
    lauxh_pushfn2tbl( L, "getaddrinfo", getaddrinfo_lua );

    return 1;
}

