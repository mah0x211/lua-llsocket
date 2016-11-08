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
 *  llsocket.h
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 14/03/29.
 *  Copyright 2014 Masatoshi Teruya. All rights reserved.
 *
 */


#ifndef ___LLSOCKET_LUA___
#define ___LLSOCKET_LUA___

#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>
// lualib
#include <lauxlib.h>
#include <lualib.h>

#include "config.h"


// memory alloc/dealloc
#define palloc(t)       (t*)malloc( sizeof(t) )
#define pnalloc(n,t)    (t*)malloc( (n) * sizeof(t) )
#define pcalloc(n,t)    (t*)calloc( n, sizeof(t) )
#define prealloc(n,t,p) (t*)realloc( p, (n) * sizeof(t) )
#define pdealloc(p)     free((void*)p)


// helper macros
#define lstate_isref(ref) \
    ((ref) >= 0)

#define lstate_ref(L) \
    luaL_ref(L,LUA_REGISTRYINDEX)

#define lstate_refat(L,idx) \
    (lua_pushvalue(L,idx),luaL_ref(L,LUA_REGISTRYINDEX))

#define lstate_pushref(L,ref) \
    lua_rawgeti( L, LUA_REGISTRYINDEX, ref )

#define lstate_unref(L,ref) \
    (luaL_unref( L, LUA_REGISTRYINDEX, ref ),LUA_NOREF)

#define lstate_setmetatable(L,label) do { \
    luaL_getmetatable( L, label ); \
    lua_setmetatable( L, -2 ); \
}while(0)

#define lstate_fn2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushcfunction(L,v); \
    lua_rawset(L,-3); \
}while(0)

#define lstate_str2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushstring(L,v); \
    lua_rawset(L,-3); \
}while(0)

#define lstate_num2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushnumber(L,v); \
    lua_rawset(L,-3); \
}while(0)

#define lstate_bool2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushboolean(L,v); \
    lua_rawset(L,-3); \
}while(0)



#define SOCKET_MT   "llsocket.socket"
#define ADDRINFO_MT "llsocket.addrinfo"


LUALIB_API int luaopen_llsocket_inet( lua_State *L );
LUALIB_API int luaopen_llsocket_unix( lua_State *L );
LUALIB_API int luaopen_llsocket_device( lua_State *L );
LUALIB_API int luaopen_llsocket_addrinfo( lua_State *L );
LUALIB_API int luaopen_llsocket_socket( lua_State *L );



static inline int lls_getaddrinfo( struct addrinfo **list, const char *node,
                                   const char *service, int family,
                                   int socktype, int protocol, int flags )
{
    struct addrinfo hints = {
        // AF_INET:ipv4 | AF_INET6:ipv6 | AF_UNSPEC
        .ai_family = family,
        // SOCK_STREAM:tcp | SOCK_DGRAM:udp | SOCK_SEQPACKET
        .ai_socktype = socktype,
        // IPPROTO_TCP:tcp | IPPROTO_UDP:udp | 0:automatic
        .ai_protocol = protocol,
        // AI_PASSIVE:bind socket if node is null
        .ai_flags = flags,
        // initialize
        .ai_addrlen = 0,
        .ai_addr = NULL,
        .ai_canonname = NULL,
        .ai_next = NULL
    };

    // getaddrinfo is better than inet_pton.
    // i wonder that can be ignore an overhead of creating socket
    // descriptor when i simply want to confirm correct address?
    // wildcard ip-address
    return getaddrinfo( node, service, &hints, list );
}


static inline struct addrinfo *lls_addrinfo_alloc( lua_State *L,
                                                   struct addrinfo *src )
{
    struct addrinfo *info = lua_newuserdata( L, sizeof( struct addrinfo ) );

    if( info )
    {
        memcpy( (void*)info, (void*)src, sizeof( struct addrinfo ) );
        info->ai_canonname = NULL;
        // copy member fields
        if( ( !src->ai_canonname ||
            ( info->ai_canonname = strdup( src->ai_canonname ) ) ) &&
            ( info->ai_addr = malloc( src->ai_addrlen ) ) ){
            info->ai_addrlen = src->ai_addrlen;
            memcpy( (void*)info->ai_addr, (void*)src->ai_addr,
                    src->ai_addrlen );
            // set metatable
            lstate_setmetatable( L, ADDRINFO_MT );
            return info;
        }
        else if( info->ai_canonname ){
            pdealloc( info->ai_canonname );
        }

        info = NULL;
    }

    return info;
}


static inline void *lls_checkudata( lua_State *L, int idx, const char *tname )
{
    const int argc = lua_gettop( L );

    if( argc >= idx )
    {
        void *udata = NULL;

        switch( lua_type( L, idx ) ){
            case LUA_TUSERDATA:
                udata = lua_touserdata( L, idx );
                // get metatable
                if( !lua_getmetatable( L, -1 ) ){
                    luaL_argerror( L, idx, "llsocket.addr expected" );
                }
                // verify metatable
                luaL_getmetatable( L, tname );
                if( !lua_rawequal( L, -1, -2 ) ){
                    luaL_argerror( L, idx, "llsocket.addr expected" );
                }
                lua_settop( L, argc );
                return udata;
        }
        luaL_argerror( L, idx, "llsocket.addr expected" );
    }

    return NULL;
}


static inline const char *lls_checklstring( lua_State *L, int idx, size_t *len )
{
    luaL_checktype( L, idx, LUA_TSTRING );

    return lua_tolstring( L, idx, len );
}


static inline const char *lls_optlstring( lua_State *L, int idx,
                                         const char *def, size_t *len )
{
    if( lua_isnoneornil( L, idx ) ){
        return def;
    }

    luaL_checktype( L, idx, LUA_TSTRING );

    return lua_tolstring( L, idx, len );
}


static inline const char *lls_checkstring( lua_State *L, int idx )
{
    luaL_checktype( L, idx, LUA_TSTRING );

    return lua_tostring( L, idx );
}


static inline const char *lls_optstring( lua_State *L, int idx,
                                         const char *def )
{
    if( lua_isnoneornil( L, idx ) ){
        return def;
    }

    luaL_checktype( L, idx, LUA_TSTRING );

    return lua_tostring( L, idx );
}


static inline lua_Integer lls_checkinteger( lua_State *L, int idx )
{
    luaL_checktype( L, idx, LUA_TNUMBER );

    return lua_tointeger( L, idx );
}


static inline lua_Integer lls_optinteger( lua_State *L, int idx,
                                          lua_Integer def )
{
    if( lua_isnoneornil( L, idx ) ){
        return def;
    }

    luaL_checktype( L, idx, LUA_TNUMBER );

    return lua_tointeger( L, idx );
}


static inline int lls_optboolean( lua_State *L, int idx, int def )
{
    if( lua_isnoneornil( L, idx ) ){
        return def > 0;
    }

    luaL_checktype( L, idx, LUA_TBOOLEAN );

    return lua_toboolean( L, idx );
}


static inline int lls_optflags( lua_State *L, int idx )
{
    const int argc = lua_gettop( L );
    int flg = 0;

    for(; idx <= argc; idx++ ){
        flg |= (int)lls_optinteger( L, idx, 0 );
    }

    return flg;
}


static inline int lls_checksockaddr( lua_State *L, int idx, int family,
                                     int socktype,
                                     struct sockaddr_storage *sockaddr )
{
    const char *str = lls_checkstring( L, idx );
    struct addrinfo *list = NULL;
    int rc = lls_getaddrinfo( &list, str, NULL, family, socktype, 0,
                              AI_NUMERICHOST );

    if( rc == 0 ){
        memcpy( (void*)sockaddr, list->ai_addr, list->ai_addrlen );
        freeaddrinfo( list );
    }

    return rc;
}


static inline int lls_check4inaddr( lua_State *L, int idx, int socktype,
                                    struct in_addr *addr )
{
    const char *str = lls_checkstring( L, idx );
    struct addrinfo *list = NULL;
    int rc = lls_getaddrinfo( &list, str, NULL, AF_INET, socktype, 0,
                              AI_NUMERICHOST );

    if( rc == 0 ){
        *addr = ((struct sockaddr_in*)list->ai_addr)->sin_addr;
        freeaddrinfo( list );
    }

    return rc;
}


static inline int lls_opt4inaddr( lua_State *L, int idx, int socktype,
                                  struct in_addr *addr, struct in_addr def )
{
    if( lua_isnoneornil( L, idx ) ){
        *addr = def;
        return 0;
    }

    return lls_check4inaddr( L, idx, socktype, addr );
}


static inline int lls_check6inaddr( lua_State *L, int idx, int socktype,
                                    struct in6_addr *addr )
{
    const char *str = lls_checkstring( L, idx );
    struct addrinfo *list = NULL;
    int rc = lls_getaddrinfo( &list, str, NULL, AF_INET6, socktype, 0,
                              AI_NUMERICHOST );

    if( rc == 0 ){
        *addr = ((struct sockaddr_in6*)list->ai_addr)->sin6_addr;
        freeaddrinfo( list );
    }

    return rc;
}


static inline int lls_opt6inaddr( lua_State *L, int idx, int socktype,
                                  struct in6_addr *addr, struct in6_addr def )
{
    if( lua_isnoneornil( L, idx ) ){
        *addr = def;
        return 0;
    }

    return lls_check6inaddr( L, idx, socktype, addr );
}


// fd option
static inline int lls_fcntl_lua( lua_State *L, int fd, int getfl, int setfl,
                                 int fl )
{
    int flg = fcntl( fd, getfl );

    if( flg != -1 )
    {
        // no args
        if( lua_isnoneornil( L, 2 ) ){
            lua_pushboolean( L, flg & fl );
            return 1;
        }

        // type check
        luaL_checktype( L, 2, LUA_TBOOLEAN );
        // set flag
        if( lua_toboolean( L, 2 ) ){
            flg |= fl;
        }
        // unset flag
        else {
            flg &= ~fl;
        }

        if( fcntl( fd, setfl, flg ) == 0 ){
            lua_pushboolean( L, flg & fl );
            return 1;
        }
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}


// socket option

static inline int lls_sockopt_int_lua( lua_State *L, int fd, int level,
                                       int opt, int type )
{
    int flg = 0;
    socklen_t len = sizeof(int);

    if( !lua_isnoneornil( L, 2 ) )
    {
        // type check
        luaL_checktype( L, 2, type );
        // set flag
        switch( type ){
            case LUA_TBOOLEAN:
                flg = (int)lua_toboolean( L, 2 );
                if( setsockopt( fd, level, opt, (void*)&flg, len ) == 0 ){
                    lua_pushboolean( L, flg );
                    return 1;
                }
            break;
            default:
                flg = (int)lua_tointeger( L, 2 );
                if( setsockopt( fd, level, opt, (void*)&flg, len ) == 0 ){
                    lua_pushinteger( L, flg );
                    return 1;
                }
        }
    }
    else if( getsockopt( fd, level, opt, (void*)&flg, &len ) == 0 )
    {
        switch( type ){
            case LUA_TBOOLEAN:
                lua_pushboolean( L, flg );
            break;
            default:
                lua_pushinteger( L, flg );
        }
        return 1;
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}


static inline int lls_sockopt_timeval_lua( lua_State *L, int fd, int level,
                                           int opt )
{
    struct timeval tval = {0,0};
    socklen_t len = sizeof( struct timeval );

    if( !lua_isnoneornil( L, 2 ) )
    {
        double tnum = (double)luaL_checknumber( L, 2 );
        double hi = 0;
        double lo = modf( tnum, &hi );

        tval.tv_sec = (time_t)hi;
        tval.tv_usec = (suseconds_t)(lo * 1000000);

        // set delay flag
        if( setsockopt( fd, level, opt, (void*)&tval, len ) == 0 ){
            lua_pushnumber( L, tnum );
            return 1;
        }
    }
    else if( getsockopt( fd, level, opt, (void*)&tval, &len ) == 0 ){
        lua_pushnumber(
            L, (double)tval.tv_sec + ((double)tval.tv_usec / 1000000)
        );
        return 1;
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}


#endif
