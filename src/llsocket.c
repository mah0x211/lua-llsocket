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
 *  llsocket.c
 *  lua-socket
 *
 *  Created by Masatoshi Teruya on 14/03/29.
 *  Copyright 2014 Masatoshi Teruya. All rights reserved.
 *
 */

#include "llsocket.h"


int fd_lua( lua_State *L )
{
    llsocket_t *s = lua_touserdata( L, 1 );
    
    lua_pushinteger( L, s->fd );
    
    return 1;
}


int close_lua( lua_State *L )
{
    llsocket_t *s = lua_touserdata( L, 1 );
    
    if( s->fd ){
        lls_close( s->fd );
        s->fd = 0;
        lua_pushboolean( L, 1 );
    }
    else {
        lua_pushboolean( L, 0 );
    }
    
    return 1;
}


int lls_inet_init( llsocket_t *s, const char *host, size_t hlen, 
                  const char *port, size_t plen, int flags, int socktype )
{
    const struct addrinfo hints = {
        // AI_PASSIVE:bind socket if node is null
        .ai_flags = flags,
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
    struct addrinfo *res = NULL;
    int rc = 0;
    
    // getaddrinfo is better than inet_pton.
    // i wonder that can be ignore an overhead of creating socket
    // descriptor when i simply want to confirm correct address?
    // wildcard ip-address
    rc = getaddrinfo( host, port, &hints, &res );
    if( rc == 0 )
    {
        struct addrinfo *ptr = res;
        int fd = 0;
        
        errno = 0;
        do
        {
            // try to create socket descriptor for find valid address
            if( ( fd = socket( ptr->ai_family, ptr->ai_socktype, 
                               ptr->ai_protocol ) ) != -1 )
            {
                struct sockaddr_in *inaddr = palloc( struct sockaddr_in );
                
                if( inaddr ){
                    s->fd = fd;
                    s->family = ptr->ai_family;
                    s->type = ptr->ai_socktype;
                    s->proto = ptr->ai_protocol;
                    s->addrlen = ptr->ai_addrlen;
                    s->addr = (void*)inaddr;
                    // copy struct sockaddr
                    memcpy( (void*)inaddr, (void*)ptr->ai_addr, 
                            (size_t)ptr->ai_addrlen );
                    // remove address-list
                    freeaddrinfo( res );
                    
                    return 0;
                }
                break;
            }

        } while( ( ptr = ptr->ai_next ) );
        
        // remove address-list
        freeaddrinfo( res );
    }
    else {
        plog( "getaddrinfo: %s:%s -> %s", host, port, gai_strerror(rc) );
    }

    
    return -1;
}


LUALIB_API int luaopen_llsocket( lua_State *L )
{
    // add methods
    lua_newtable( L );
    luaopen_llsocket_inet( L );
    lua_setfield( L, -2, "inet" );
    
    return 1;
}

