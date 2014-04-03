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
 *  socket.h
 *  lua-socket
 *
 *  Created by Masatoshi Teruya on 14/03/29.
 *  Copyright 2014 Masatoshi Teruya. All rights reserved.
 *
 */


#ifndef ___LLSOCKET_LUA___
#define ___LLSOCKET_LUA___

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
// lualib
#include <lauxlib.h>
#include <lualib.h>


// memory alloc/dealloc
#define palloc(t)       (t*)malloc( sizeof(t) )
#define pnalloc(n,t)    (t*)malloc( (n) * sizeof(t) )
#define pcalloc(n,t)    (t*)calloc( n, sizeof(t) )
#define prealloc(n,t,p) (t*)realloc( p, (n) * sizeof(t) )
#define pdealloc(p)     free((void*)p)


// print message to stdout
#define plog(fmt,...) \
    printf( fmt "\n", ##__VA_ARGS__ )

#define pflog(f,fmt,...) \
    printf( #f "(): " fmt "\n", ##__VA_ARGS__ )

// print message to stderr
#define pelog(fmt,...) \
    fprintf( stderr, fmt "\n", ##__VA_ARGS__ )

#define pfelog(f,fmt,...) \
    fprintf( stderr, #f "(): " fmt "\n", ##__VA_ARGS__ )

// print message to stderr with strerror
#define pelogerr(fmt,...) \
    fprintf( stderr, fmt " : %s\n", ##__VA_ARGS__, strerror(errno) )

#define pfelogerr(f,fmt,...) \
    fprintf( stderr, #f "(): " fmt " : %s\n", ##__VA_ARGS__, strerror(errno) )


// helper macros
#define lstate_setmetatable(L,label) do { \
    luaL_getmetatable( L, label ); \
    lua_setmetatable( L, -2 ); \
}while(0)


#define lstate_fn2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushcfunction(L,v); \
    lua_rawset(L,-3); \
}while(0)



// define data structures

#define DECLARE_LLS_STRUCT_BASE \
    /* socket descriptor */ \
    int32_t fd; \
    /* type of address family */ \
    int family; \
    /* socket type: SOCK_STREAM|SOCK_DGRAM|SOCK_SEQPACKET */ \
    int type; \
    /* type of protocol: AF_UNIX|AF_INET */ \
    int proto; \
    /* opaque address byte length */ \
    size_t addrlen

typedef struct {
    DECLARE_LLS_STRUCT_BASE;
    struct sockaddr addr;
} llsocket_t;

typedef struct {
    DECLARE_LLS_STRUCT_BASE;
    struct sockaddr_in addr;
} lls_inet_t;


// modules
LUALIB_API int luaopen_llsocket( lua_State *L );


// module definition register
static inline int lls_define_mt( lua_State *L, const char *tname, 
                                 struct luaL_Reg mmethod[], 
                                 struct luaL_Reg method[] )
{
    int i;
    
    // create table __metatable
    luaL_newmetatable( L, tname );
    // metamethods
    i = 0;
    while( mmethod[i].name ){
        lstate_fn2tbl( L, mmethod[i].name, mmethod[i].func );
        i++;
    }
    // methods
    lua_pushstring( L, "__index" );
    lua_newtable( L );
    i = 0;
    while( method[i].name ){
        lstate_fn2tbl( L, method[i].name, method[i].func );
        i++;
    }
    lua_rawset( L, -3 );
    
    return 1;
}


// fd helper
#define lls_set_cloexec(fd) fcntl(fd, F_SETFD, FD_CLOEXEC)

static inline int lls_set_nonblock( int fd ){
    int flg = fcntl( fd, F_GETFL );
    return fcntl( fd, F_SETFL, flg|O_NONBLOCK );
}

static inline int lls_set_reuseaddr( int fd ){
    int flg = 1;
    return setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, &flg, sizeof(int) );
}


// shared methods
#define lls_tostring(L,meta) ({ \
    lua_pushfstring( L, meta ": %p", lua_touserdata( L, 1 ) ); \
    1; \
})

static inline int lls_fd( lua_State *L, const char *tname )
{
    llsocket_t *s = luaL_checkudata( L, 1, tname );
    
    lua_pushinteger( L, s->fd );
    
    return 1;
}

static inline int lls_close( lua_State *L, const char *tname )
{
    llsocket_t *s = luaL_checkudata( L, 1, tname );
    
    if( s->fd ){
        close( s->fd );
        s->fd = 0;
        lua_pushboolean( L, 1 );
    }
    else {
        lua_pushboolean( L, 0 );
    }
    
    return 1;
}


static inline int lls_connect( lua_State *L, const char *tname )
{
    llsocket_t *s = luaL_checkudata( L, 1, tname );
    int rc = connect( s->fd, &s->addr, (socklen_t)s->addrlen );
    
    if( rc == 0 ){
        lua_pushboolean( L, 1 );
        return 1;
    }
    else if( errno == EINPROGRESS ){
        lua_pushboolean( L, 0 );
        return 1;
    }
    
    // got error
    lua_pushboolean( L, 0 );
    lua_pushinteger( L, errno );
    
    return 2;
}


static inline int lls_bind( lua_State *L, const char *tname )
{
    llsocket_t *s = luaL_checkudata( L, 1, tname );
    
    // reuseaddr and bind
    if( lls_set_reuseaddr( s->fd ) != -1 &&
        bind( s->fd, &s->addr, (socklen_t)s->addrlen ) == 0 ){
        lua_pushboolean( L, 1 );
        return 1;
    }
    
    // got error
    lua_pushboolean( L, 0 );
    lua_pushinteger( L, errno );
    
    return 2;
}


#endif
