/**
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

static int getnameinfo_lua(lua_State *L)
{
    lls_addrinfo_t *info = lauxh_checkudata(L, 1, ADDRINFO_MT);
    int flag             = lauxh_optflags(L, 2);
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    int rc = getnameinfo(info->ai.ai_addr, info->ai.ai_addrlen, host,
                         NI_MAXHOST, serv, NI_MAXSERV, flag);

    // got error
    if (rc != 0) {
        lua_pushnil(L);
        lua_errno_eai_new(L, rc, "getnameinfo");
        return 2;
    }

    lua_createtable(L, 0, 2);
    lauxh_pushstr2tbl(L, "host", host);
    lauxh_pushstr2tbl(L, "service", serv);

    return 1;
}

static int addr_lua(lua_State *L)
{
    lls_addrinfo_t *info       = lauxh_checkudata(L, 1, ADDRINFO_MT);
    char buf[INET6_ADDRSTRLEN] = {0};

    switch (info->ai.ai_family) {
    case AF_INET: {
        struct sockaddr_in *addr = (struct sockaddr_in *)info->ai.ai_addr;
        lua_pushstring(L, inet_ntop(AF_INET, (const void *)&addr->sin_addr, buf,
                                    INET6_ADDRSTRLEN));
    } break;

    case AF_INET6: {
        struct sockaddr_in6 *addr = (struct sockaddr_in6 *)info->ai.ai_addr;
        lua_pushstring(L, inet_ntop(AF_INET6, (const void *)&addr->sin6_addr,
                                    buf, INET6_ADDRSTRLEN));
    } break;

    case AF_UNIX: {
        struct sockaddr_un *addr = (struct sockaddr_un *)info->ai.ai_addr;
        lua_pushstring(L, addr->sun_path);
    } break;

    // unsupported family
    default:
        lua_pushnil(L);
    }

    return 1;
}

static int port_lua(lua_State *L)
{
    lls_addrinfo_t *info = lauxh_checkudata(L, 1, ADDRINFO_MT);

    switch (info->ai.ai_family) {
    case AF_INET: {
        struct sockaddr_in *addr = (struct sockaddr_in *)info->ai.ai_addr;
        lua_pushinteger(L, ntohs(addr->sin_port));
    } break;

    case AF_INET6: {
        struct sockaddr_in6 *addr = (struct sockaddr_in6 *)info->ai.ai_addr;
        lua_pushinteger(L, ntohs(addr->sin6_port));
    } break;

    // unsupported family
    default:
        lua_pushnil(L);
    }

    return 1;
}

static int canonname_lua(lua_State *L)
{
    lls_addrinfo_t *info = lauxh_checkudata(L, 1, ADDRINFO_MT);

    if (info->ai.ai_canonname) {
        lua_pushstring(L, info->ai.ai_canonname);
        return 1;
    }

    return 0;
}

static int protocol_lua(lua_State *L)
{
    lls_addrinfo_t *info = lauxh_checkudata(L, 1, ADDRINFO_MT);

    lua_pushinteger(L, info->ai.ai_protocol);

    return 1;
}

static int socktype_lua(lua_State *L)
{
    lls_addrinfo_t *info = lauxh_checkudata(L, 1, ADDRINFO_MT);

    lua_pushinteger(L, info->ai.ai_socktype);

    return 1;
}

static int family_lua(lua_State *L)
{
    lls_addrinfo_t *info = lauxh_checkudata(L, 1, ADDRINFO_MT);

    lua_pushinteger(L, info->ai.ai_family);

    return 1;
}

static int tostring_lua(lua_State *L)
{
    lua_pushfstring(L, ADDRINFO_MT ": %p", lua_touserdata(L, 1));
    return 1;
}

static int gc_lua(lua_State *L)
{
    lls_addrinfo_t *info = lauxh_checkudata(L, 1, ADDRINFO_MT);

    info->ai_addr_ref      = lauxh_unref(L, info->ai_addr_ref);
    info->ai_canonname_ref = lauxh_unref(L, info->ai_canonname_ref);

    return 0;
}

static int getaddrinfo_lua(lua_State *L)
{
    size_t nodelen        = 0;
    const char *node      = lauxh_optlstring(L, 1, NULL, &nodelen);
    size_t servicelen     = 0;
    const char *service   = lauxh_optlstring(L, 2, NULL, &servicelen);
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
    int rc                = lls_getaddrinfo(&list, (nodelen) ? node : NULL,
                             (servicelen) ? service : NULL, family, socktype,
                                            protocol, flags);

    if (rc != 0) {
        lua_pushnil(L);
        lua_errno_eai_new(L, rc, "getaddrinfo");
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

static int inet6_lua(lua_State *L)
{
    size_t len                = 0;
    const char *addr          = lauxh_optlstring(L, 1, NULL, &len);
    uint16_t port             = lauxh_optuint16(L, 2, 0);
    struct sockaddr_in6 saddr = {.sin6_family = AF_INET6,
                                 .sin6_port   = htons(port),
                                 .sin6_addr   = in6addr_any};
    struct addrinfo ai        = {.ai_family    = AF_INET6,
                                 // SOCK_STREAM:tcp | SOCK_DGRAM:udp | SOCK_SEQPACKET
                                 .ai_socktype  = (int)lauxh_optinteger(L, 3, 0),
                                 // IPPROTO_TCP:tcp | IPPROTO_UDP:udp | 0:automatic
                                 .ai_protocol  = (int)lauxh_optinteger(L, 4, 0),
                                 // AI_PASSIVE:bind socket if node is null
                                 .ai_flags     = (int)lauxh_optflags(L, 5),
                                 // initialize
                                 .ai_addrlen   = sizeof(saddr),
                                 .ai_addr      = (struct sockaddr *)&saddr,
                                 .ai_canonname = NULL,
                                 .ai_next      = NULL};

#ifdef HAVE_SOCKADDR_SA_LEN
    saddr.sin6_len = sizeof(saddr);
#endif

    if (len) {
        switch (inet_pton(AF_INET6, addr, (void *)&saddr.sin6_addr)) {
        case -1:
            lua_pushnil(L);
            lua_errno_new(L, errno, "inet_pton");
            return 2;
        case 0:
            // addr cannot be parsed as ipv6 address
            lua_pushnil(L);
            errno = EAFNOSUPPORT;
            lua_errno_new(L, errno, "inet_pton");
            return 2;
        }
    }
    lls_addrinfo_alloc(L, &ai);

    return 1;
}

static int inet_lua(lua_State *L)
{
    size_t len               = 0;
    const char *addr         = lauxh_optlstring(L, 1, NULL, &len);
    uint16_t port            = lauxh_optuint16(L, 2, 0);
    struct sockaddr_in saddr = {.sin_family = AF_INET,
                                .sin_port   = htons(port),
                                .sin_addr   = {.s_addr = INADDR_ANY}};
    struct addrinfo ai       = {.ai_family    = AF_INET,
                                // SOCK_STREAM:tcp | SOCK_DGRAM:udp | SOCK_SEQPACKET
                                .ai_socktype  = (int)lauxh_optinteger(L, 3, 0),
                                // IPPROTO_TCP:tcp | IPPROTO_UDP:udp | 0:automatic
                                .ai_protocol  = (int)lauxh_optinteger(L, 4, 0),
                                // AI_PASSIVE:bind socket if node is null
                                .ai_flags     = (int)lauxh_optflags(L, 5),
                                // initialize
                                .ai_addrlen   = sizeof(saddr),
                                .ai_addr      = (struct sockaddr *)&saddr,
                                .ai_canonname = NULL,
                                .ai_next      = NULL};

#ifdef HAVE_SOCKADDR_SA_LEN
    saddr.sin_len = sizeof(saddr);
#endif

    if (len) {
        switch (inet_pton(AF_INET, addr, (void *)&saddr.sin_addr)) {
        case -1:
            lua_pushnil(L);
            lua_errno_new(L, errno, "inet_pton");
            return 2;
        case 0:
            // addr cannot be parsed as ipv4 address
            lua_pushnil(L);
            errno = EAFNOSUPPORT;
            lua_errno_new(L, errno, "inet_pton");
            return 2;
        }
    }
    lls_addrinfo_alloc(L, &ai);

    return 1;
}

static int unix_lua(lua_State *L)
{
    size_t len               = 0;
    const char *pathname     = lauxh_checklstring(L, 1, &len);
    struct sockaddr_un saddr = {.sun_family = AF_UNIX, .sun_path = {0}};
    struct addrinfo ai       = {.ai_family    = AF_UNIX,
                                // SOCK_STREAM:tcp | SOCK_DGRAM:udp | SOCK_SEQPACKET
                                .ai_socktype  = (int)lauxh_optinteger(L, 2, 0),
                                // IPPROTO_TCP:tcp | IPPROTO_UDP:udp | 0:automatic
                                .ai_protocol  = (int)lauxh_optinteger(L, 3, 0),
                                // AI_PASSIVE:bind socket if node is null
                                .ai_flags     = (int)lauxh_optflags(L, 4),
                                // initialize
                                .ai_addrlen   = sizeof(saddr),
                                .ai_addr      = (struct sockaddr *)&saddr,
                                .ai_canonname = NULL,
                                .ai_next      = NULL};

    // length too large
    if (len >= UNIXPATH_MAX) {
        lua_pushnil(L);
        errno = ENAMETOOLONG;
        lua_errno_new(L, errno, "unix");
        return 2;
    }
    memcpy((void *)&saddr.sun_path, (void *)pathname, len);
    saddr.sun_path[len] = 0;
    lls_addrinfo_alloc(L, &ai);

    return 1;
}

LUALIB_API int luaopen_llsocket_addrinfo(lua_State *L)
{
    // create metatable
    if (luaL_newmetatable(L, ADDRINFO_MT)) {
        struct luaL_Reg mmethod[] = {
            {"__gc",       gc_lua      },
            {"__tostring", tostring_lua},
            {NULL,         NULL        }
        };
        struct luaL_Reg method[] = {
            {"family",      family_lua     },
            {"socktype",    socktype_lua   },
            {"protocol",    protocol_lua   },
            {"canonname",   canonname_lua  },
            {"port",        port_lua       },
            {"addr",        addr_lua       },
            {"getnameinfo", getnameinfo_lua},
            {NULL,          NULL           }
        };
        struct luaL_Reg *ptr = mmethod;

        // metamethods
        do {
            lauxh_pushfn2tbl(L, ptr->name, ptr->func);
            ptr++;
        } while (ptr->name);
        // methods
        lua_pushstring(L, "__index");
        lua_newtable(L);
        ptr = method;
        do {
            lauxh_pushfn2tbl(L, ptr->name, ptr->func);
            ptr++;
        } while (ptr->name);
        lua_rawset(L, -3);
    }
    lua_pop(L, 1);

    // create table
    lua_newtable(L);
    lauxh_pushfn2tbl(L, "unix", unix_lua);
    lauxh_pushfn2tbl(L, "inet", inet_lua);
    lauxh_pushfn2tbl(L, "inet6", inet6_lua);
    lauxh_pushfn2tbl(L, "getaddrinfo", getaddrinfo_lua);

    return 1;
}
