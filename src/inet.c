/**
 *  Copyright (C) 2014 Masatoshi Teruya
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *
 *  inet.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 14/03/29.
 */

#include "llsocket.h"

static int getaddrinfo_lua(lua_State *L)
{
    const char *node      = lauxh_optstring(L, 1, NULL);
    const char *service   = lauxh_optstring(L, 2, NULL);
    int family            = (int)lauxh_optinteger(L, 3, AF_UNSPEC);
    // SOCK_STREAM:tcp | SOCK_DGRAM:udp | SOCK_SEQPACKET
    int socktype          = (int)lauxh_optinteger(L, 4, 0);
    // IPPROTO_TCP:tcp | IPPROTO_UDP:udp | 0:automatic
    int protocol          = (int)lauxh_optinteger(L, 5, 0);
    // AI_PASSIVE:bind socket if node is null
    int flags             = lauxh_optflags(L, 6);
    struct addrinfo *list = NULL;
    struct addrinfo *ptr  = NULL;
    int idx               = 1;
    int rc = lls_getaddrinfo(&list, node, service, family, socktype, protocol,
                             flags);

    if (rc != 0) {
        lua_pushnil(L);
        lua_pushstring(L, gai_strerror(rc));
        return 2;
    }

    // create address table
    lua_newtable(L);
    ptr = list;
    while (ptr) {
        lls_addrinfo_alloc(L, ptr);
        lua_rawseti(L, -2, idx++);
        // check next
        ptr = ptr->ai_next;
    }

    // remove address-list
    freeaddrinfo(list);

    return 1;
}

LUALIB_API int luaopen_llsocket_inet(lua_State *L)
{
    lua_newtable(L);
    lauxh_pushfn2tbl(L, "getaddrinfo", getaddrinfo_lua);

    return 1;
}
