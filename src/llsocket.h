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
// lualib
#include <lauxlib.h>
#include <lualib.h>


// linux 2.6.27
#if defined(SOCK_CLOEXEC)
#define LINUX_SOCKEXT   1
#endif

// linux 2.6.28
#if defined(SYS_ACCEPT4)
#define LINUX_ACCEPT4   1
#endif


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



LUALIB_API int luaopen_llsocket_inet( lua_State *L );
LUALIB_API int luaopen_llsocket_unix( lua_State *L );


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
    lua_pushinteger( L, errno );
    
    return 2;
}

// socket option
static inline int lls_sockopt_int_lua( lua_State *L, int fd, int level, int opt, 
                                       int type )
{
    int flg = 0;
    socklen_t len = sizeof(int);
    
    if( getsockopt( fd, level, opt, (void*)&flg, &len ) == 0 )
    {
        // no args
        if( lua_isnoneornil( L, 2 ) ){
            switch( type ){
                case LUA_TBOOLEAN:
                    lua_pushboolean( L, flg );
                break;
                default:
                    lua_pushinteger( L, flg );
            }
            return 1;
        }
        
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
    
    // got error
    lua_pushnil( L );
    lua_pushinteger( L, errno );
    
    return 2;
}


static inline int lls_sockopt_timeval_lua( lua_State *L, int fd, int level, 
                                           int opt )
{
    struct timeval tval = {0};
    socklen_t len = sizeof( struct timeval );
    
    if( getsockopt( fd, level, opt, (void*)&tval, &len ) == 0 )
    {
        double tnum, hi, lo;
        
        // no args
        if( lua_isnoneornil( L, 2 ) ){
            tnum = (double)tval.tv_sec + ((double)tval.tv_usec / 1000000);
            goto SUCCESS;
        }
        
        // type check
        tnum = (double)luaL_checknumber( L, 2 );
        lo = modf( tnum, &hi );
        tval.tv_sec = (time_t)hi;
        tval.tv_usec = (suseconds_t)(lo * 1000000);
    
        // set delay flag
        if( setsockopt( fd, level, opt, (void*)&tval, len ) == 0 ){
SUCCESS:
            lua_pushnumber( L, tnum );
            return 1;
        }
    }
    
    // got error
    lua_pushnil( L );
    lua_pushinteger( L, errno );
    
    return 2;
}


#endif
