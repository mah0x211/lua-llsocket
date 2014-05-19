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


typedef int(*connbind_t)( int, const struct sockaddr*, socklen_t );

static int connbind_lua( lua_State *L, connbind_t proc, int passive )
{
    const char *host = lua_tostring( L, 1 );
    const char *port = lua_tostring( L, 2 );
    int socktype = luaL_checkint( L, 3 );
    int nonblock = 0;
    int reuseaddr = 0;
    const struct addrinfo hints = {
        // AI_PASSIVE:bind socket if node is null
        .ai_flags = passive,
        // AF_INET:ipv4 | AF_INET6:ipv6
        .ai_family = AF_UNSPEC,
        // SOCK_STREAM:tcp | SOCK_DGRAM:udp | SOCK_SEQPACKET
        .ai_socktype = socktype,
        // IPPROTO_TCP:tcp | IPPROTO_UDP:udp | 0:automatic
        .ai_protocol = 0,
        // initialize
        .ai_addrlen = 0,
        .ai_canonname = NULL,
        .ai_addr = NULL,
        .ai_next = NULL
    };
    struct addrinfo *list = NULL;
    
    // check arguments
    // host, port
    if( !host && !port ){
        luaL_error( L, "must be specified either host, port or both" );
    }
    // nonblock
    if( !lua_isnoneornil( L, 4 ) )
    {
        luaL_checktype( L, 4, LUA_TBOOLEAN );
#if defined(LINUX_SOCKEXT)
        if( lua_toboolean( L, 4 ) ){
            nonblock = SOCK_NONBLOCK;
        }
#else
        nonblock = lua_toboolean( L, 4 );
#endif
    }
    // reuseaddr
    if( !lua_isnoneornil( L, 5 ) ){
        luaL_checktype( L, 5, LUA_TBOOLEAN );
        reuseaddr = lua_toboolean( L, 5 );
    }
    
    // getaddrinfo is better than inet_pton.
    // i wonder that can be ignore an overhead of creating socket
    // descriptor when i simply want to confirm correct address?
    // wildcard ip-address
    if( getaddrinfo( host, port, (const struct addrinfo*)&hints, &list ) != -1 )
    {
        struct addrinfo *ptr = list;
        int fd = 0;
        
        do
        {
            // try to create socket
#if defined(LINUX_SOCKEXT)
            fd = socket( ptr->ai_family, 
                         ptr->ai_socktype|SOCK_CLOEXEC|nonblock, 
                         ptr->ai_protocol );
#else
            fd = socket( ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol );
#endif
            if( fd != -1 )
            {
                if( reuseaddr ){
                    setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, 
                                (void*)&reuseaddr, sizeof(int) );
                }
                
#if !defined(LINUX_SOCKEXT)
                fcntl( fd, F_SETFD, FD_CLOEXEC );
                if( nonblock ){
                    int fl = fcntl( fd, F_GETFL );
                    fcntl( fd, F_SETFL, fl|O_NONBLOCK );
                }
#endif
                if( proc( fd, ptr->ai_addr, ptr->ai_addrlen ) == 0 ||
                    // nonblocking connect
                    ( proc == connect && errno == EINPROGRESS ) ){
                    break;
                }
                close( fd );
                fd = -1;
            }
            
        } while( ( ptr = ptr->ai_next ) );
        
        // remove address-list
        freeaddrinfo( list );
        
        if( fd != -1 ){
            lua_pushinteger( L, fd );
            return 1;
        }
    }
    
    // got error
    lua_pushnil( L );
    lua_pushinteger( L, errno );
    
    return 2;
}


// method
static int connect_lua( lua_State *L )
{
    return connbind_lua( L, connect, 0 );
}

static int bind_lua( lua_State *L )
{
    return connbind_lua( L, bind, AI_PASSIVE );
}

LUALIB_API int luaopen_llsocket_inet( lua_State *L )
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

