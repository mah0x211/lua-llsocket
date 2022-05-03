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
 *  llsocket.h
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 14/03/29.
 */

#ifndef ___LLSOCKET_LUA___
#define ___LLSOCKET_LUA___

#define _GNU_SOURCE

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <math.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
// lualib
#include "config.h"
#include "lua_error.h"
#include "lua_iovec.h"
#include <lauxhlib.h>

#define SOCKET_MT   "llsocket.socket"
#define ADDRINFO_MT "llsocket.addrinfo"
#define CMSGHDR_MT  "llsocket.cmsghdr"
#define CMSGHDRS_MT "llsocket.cmsghdrs"
#define MSGHDR_MT   "llsocket.msghdr"

#if defined(__linux__)
# include <linux/if.h>
# include <linux/if_packet.h>
#else
# include <net/if_dl.h>
#endif

// unix-domain socket max path length
#define UNIXPATH_MAX (sizeof(((struct sockaddr_un *)0)->sun_path))

LUALIB_API int luaopen_llsocket(lua_State *L);
LUALIB_API int luaopen_llsocket_inet(lua_State *L);
LUALIB_API int luaopen_llsocket_unix(lua_State *L);
LUALIB_API int luaopen_llsocket_device(lua_State *L);
LUALIB_API int luaopen_llsocket_addrinfo(lua_State *L);
LUALIB_API int luaopen_llsocket_socket(lua_State *L);
LUALIB_API int luaopen_llsocket_iovec(lua_State *L);
LUALIB_API int luaopen_llsocket_cmsghdr(lua_State *L);
LUALIB_API int luaopen_llsocket_cmsghdrs(lua_State *L);
LUALIB_API int luaopen_llsocket_msghdr(lua_State *L);
LUALIB_API int luaopen_llsocket_env(lua_State *L);

#define ERROR_TYPE_NAME "llsocket.error"

static inline void lls_initerror(lua_State *L)
{
    le_loadlib(L, 1);
    lua_getfield(L, LUA_REGISTRYINDEX, ERROR_TYPE_NAME);
    if (lua_isnil(L, -1)) {
        lua_pushliteral(L, ERROR_TYPE_NAME);
        le_new_type(L, -1);
        lua_setfield(L, LUA_REGISTRYINDEX, ERROR_TYPE_NAME);
    }
    lua_pop(L, 1);
}

static inline void lls_pusherror_ex(lua_State *L, const char *msg,
                                    const char *op, int code, int wrapidx)
{
    int top = lua_gettop(L);

    if (wrapidx < 0) {
        wrapidx = top + wrapidx + 1;
    }

    lua_getfield(L, LUA_REGISTRYINDEX, ERROR_TYPE_NAME);
    lua_pushstring(L, msg);
    lua_pushstring(L, op);
    lua_pushinteger(L, code);
    le_new_message(L, top + 2);
    if (wrapidx) {
        lua_pushvalue(L, wrapidx);
    }
    le_new_typed_error(L, top + 1);
}

static inline void lls_pusherror(lua_State *L, const char *msg, const char *op,
                                 int code)
{
    lls_pusherror_ex(L, msg, op, code, 0);
}

typedef struct {
    int ref;
    // originating protocol
    int level;
    // protocol-specific type
    int type;
    // data byte count, not including header
    int len;
    const char *data;
} lls_cmsghdr_t;

typedef struct {
    int ref;
    size_t bytes;
    size_t len;
    char *data;
} lls_cmsghdrs_t;

typedef struct {
    // msg_name
    int name_ref;
    // msg_iov
    int iov_ref;
    // msg_control
    int control_ref;
    // msg_flags
    int flags;
    // data pointers
    struct addrinfo *name;
    lua_iovec_t *iov;
    lls_cmsghdrs_t *control;
} lls_msghdr_t;

typedef struct {
    int ai_addr_ref;
    int ai_canonname_ref;
    struct addrinfo ai;
} lls_addrinfo_t;

static inline lls_cmsghdr_t *lls_cmsghdr_alloc(lua_State *L, int level,
                                               int type)
{
    size_t len          = 0;
    const char *data    = lauxh_checklstring(L, -1, &len);
    lls_cmsghdr_t *cmsg = lua_newuserdata(L, sizeof(lls_cmsghdr_t));

    lua_pushvalue(L, -2);
    cmsg->ref   = lauxh_ref(L);
    cmsg->level = level;
    cmsg->type  = type;
    cmsg->len   = len;
    cmsg->data  = data;
    lauxh_setmetatable(L, CMSGHDR_MT);

    return cmsg;
}

static inline int lls_getaddrinfo(struct addrinfo **list, const char *node,
                                  const char *service, int family, int socktype,
                                  int protocol, int flags)
{
    struct addrinfo hints = {
        // AF_INET:ipv4 | AF_INET6:ipv6 | AF_UNSPEC
        .ai_family    = family,
        // SOCK_STREAM:tcp | SOCK_DGRAM:udp | SOCK_SEQPACKET
        .ai_socktype  = socktype,
        // IPPROTO_TCP:tcp | IPPROTO_UDP:udp | 0:automatic
        .ai_protocol  = protocol,
        // AI_PASSIVE:bind socket if node is null
        .ai_flags     = flags,
        // initialize
        .ai_addrlen   = 0,
        .ai_addr      = NULL,
        .ai_canonname = NULL,
        .ai_next      = NULL};

    // getaddrinfo is better than inet_pton.
    // i wonder that can be ignore an overhead of creating socket
    // descriptor when i simply want to confirm correct address?
    // wildcard ip-address
    return getaddrinfo(node, service, &hints, list);
}

static inline lls_addrinfo_t *lls_addrinfo_alloc(lua_State *L,
                                                 struct addrinfo *src)
{
    lls_addrinfo_t *info = lua_newuserdata(L, sizeof(lls_addrinfo_t));

    // copy data
    memcpy((void *)&info->ai, (void *)src, sizeof(struct addrinfo));
    info->ai.ai_addr    = lua_newuserdata(L, sizeof(struct sockaddr_storage));
    info->ai_addr_ref   = lauxh_ref(L);
    info->ai.ai_addrlen = src->ai_addrlen;
    memcpy((void *)info->ai.ai_addr, (void *)src->ai_addr, src->ai_addrlen);
    info->ai.ai_canonname  = NULL;
    info->ai_canonname_ref = LUA_NOREF;
    if (!src->ai_canonname) {
        lua_pushstring(L, src->ai_canonname);
        info->ai.ai_canonname  = (char *)lua_tostring(L, -1);
        info->ai_canonname_ref = lauxh_ref(L);
    }
    // set metatable
    lauxh_setmetatable(L, ADDRINFO_MT);

    return info;
}

static inline void *lls_checkudata(lua_State *L, int idx, const char *tname)
{
    const int argc = lua_gettop(L);

    if (argc >= idx) {
        void *udata = NULL;

        switch (lua_type(L, idx)) {
        case LUA_TUSERDATA:
            udata = lua_touserdata(L, idx);
            // get metatable
            if (!lua_getmetatable(L, -1)) {
                luaL_argerror(L, idx, "llsocket.addr expected");
            }
            // verify metatable
            luaL_getmetatable(L, tname);
            if (!lua_rawequal(L, -1, -2)) {
                luaL_argerror(L, idx, "llsocket.addr expected");
            }
            lua_settop(L, argc);
            return udata;
        }
        luaL_argerror(L, idx, "llsocket.addr expected");
    }

    return NULL;
}

static inline int lls_checksockaddr(lua_State *L, int idx, int family,
                                    int socktype,
                                    struct sockaddr_storage *sockaddr)
{
    const char *str       = lauxh_checkstring(L, idx);
    struct addrinfo *list = NULL;
    int rc =
        lls_getaddrinfo(&list, str, NULL, family, socktype, 0, AI_NUMERICHOST);

    if (rc == 0) {
        memcpy((void *)sockaddr, list->ai_addr, list->ai_addrlen);
        freeaddrinfo(list);
    }

    return rc;
}

static inline int lls_check4inaddr(lua_State *L, int idx, int socktype,
                                   struct in_addr *addr)
{
    const char *str       = lauxh_checkstring(L, idx);
    struct addrinfo *list = NULL;
    int rc =
        lls_getaddrinfo(&list, str, NULL, AF_INET, socktype, 0, AI_NUMERICHOST);

    if (rc == 0) {
        *addr = ((struct sockaddr_in *)list->ai_addr)->sin_addr;
        freeaddrinfo(list);
    }

    return rc;
}

static inline int lls_opt4inaddr(lua_State *L, int idx, int socktype,
                                 struct in_addr *addr, struct in_addr def)
{
    if (lua_isnoneornil(L, idx)) {
        *addr = def;
        return 0;
    }

    return lls_check4inaddr(L, idx, socktype, addr);
}

static inline int lls_check6inaddr(lua_State *L, int idx, int socktype,
                                   struct in6_addr *addr)
{
    const char *str       = lauxh_checkstring(L, idx);
    struct addrinfo *list = NULL;
    int rc = lls_getaddrinfo(&list, str, NULL, AF_INET6, socktype, 0,
                             AI_NUMERICHOST);

    if (rc == 0) {
        *addr = ((struct sockaddr_in6 *)list->ai_addr)->sin6_addr;
        freeaddrinfo(list);
    }

    return rc;
}

static inline int lls_opt6inaddr(lua_State *L, int idx, int socktype,
                                 struct in6_addr *addr, struct in6_addr def)
{
    if (lua_isnoneornil(L, idx)) {
        *addr = def;
        return 0;
    }

    return lls_check6inaddr(L, idx, socktype, addr);
}

// fd option
static inline int lls_fcntl_lua(lua_State *L, int fd, int getfl, int setfl,
                                int fl)
{
    int top = lua_gettop(L);
    int flg = fcntl(fd, getfl);

    if (flg == -1) {
        lua_pushnil(L);
        lls_pusherror(L, strerror(errno), "fcntl", errno);
        return 2;
    }

    // push current value
    lua_pushboolean(L, flg & fl);
    // no-change
    if (top == 1 || lua_isnoneornil(L, 2)) {
        return 1;
    }

    // change
    luaL_checktype(L, 2, LUA_TBOOLEAN);
    if (lua_toboolean(L, 2)) {
        // set flag
        flg |= fl;
    } else {
        // unset flag
        flg &= ~fl;
    }
    if (fcntl(fd, setfl, flg) != 0) {
        lua_pushnil(L);
        lls_pusherror(L, strerror(errno), "fcntl", errno);
        return 2;
    }

    return 1;
}

// socket option

static inline int lls_sockopt_int_lua(lua_State *L, int fd, int level, int opt,
                                      int type)
{
    int top       = lua_gettop(L);
    int flg       = 0;
    socklen_t len = sizeof(int);

    if (getsockopt(fd, level, opt, (void *)&flg, &len) != 0) {
        lua_pushnil(L);
        lls_pusherror(L, strerror(errno), "getsockopt", errno);
        return 2;
    }

    switch (type) {
    case LUA_TBOOLEAN:
        lua_pushboolean(L, flg);
        break;

    default:
        lua_pushinteger(L, flg);
    }

    // no-change
    if (top == 1 || lua_isnoneornil(L, 2)) {
        return 1;
    }

    // type check
    luaL_checktype(L, 2, type);
    switch (type) {
    case LUA_TBOOLEAN:
        flg = (int)lua_toboolean(L, 2);
        if (setsockopt(fd, level, opt, (void *)&flg, len) == 0) {
            return 1;
        }
        break;

    default:
        flg = (int)lua_tointeger(L, 2);
        if (setsockopt(fd, level, opt, (void *)&flg, len) == 0) {
            return 1;
        }
    }

    // got error
    lua_pushnil(L);
    lls_pusherror(L, strerror(errno), "setsockopt", errno);

    return 2;
}

static inline int lls_sockopt_timeval_lua(lua_State *L, int fd, int level,
                                          int opt)
{
    int top             = lua_gettop(L);
    struct timeval tval = {0, 0};
    socklen_t len       = sizeof(struct timeval);

    if (getsockopt(fd, level, opt, (void *)&tval, &len) != 0) {
        lua_pushnil(L);
        lls_pusherror(L, strerror(errno), "getsockopt", errno);
        return 2;
    }

    lua_pushnumber(L, (double)tval.tv_sec + ((double)tval.tv_usec / 1000000));
    // change
    if (top != 1 && !lua_isnoneornil(L, 2)) {
        double tnum = (double)luaL_checknumber(L, 2);
        double hi   = 0;
        double lo   = modf(tnum, &hi);

        tval.tv_sec  = (time_t)hi;
        tval.tv_usec = (suseconds_t)(lo * 1000000);
        // set delay flag
        if (setsockopt(fd, level, opt, (void *)&tval, len) != 0) {
            lua_pushnil(L);
            lls_pusherror(L, strerror(errno), "setsockopt", errno);
            return 2;
        }
    }

    return 1;
}

#endif
