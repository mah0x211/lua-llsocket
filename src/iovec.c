/*
 *  Copyright (C) 2017 Masatoshi Teruya
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
 *  iovec.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 17/08/14.
 */

#include "llsocket.h"


static int del_lua( lua_State *L )
{
    liovec_t *iov = lauxh_checkudata( L, 1, IOVEC_MT );
    lua_Integer idx = lauxh_checkinteger( L, 2 );

    if( idx < iov->used && idx >= 0 )
    {
        lauxh_pushref( L, iov->refs[idx] );
        lauxh_unref( L, iov->refs[idx] );
        iov->used--;
        if( iov->used != idx )
        {
            // move the last data to idx to fill in hole of array
            iov->refs[idx] = iov->refs[iov->used];
            iov->data[idx] = (struct iovec){
                .iov_base = iov->data[iov->used].iov_base,
                .iov_len = iov->data[iov->used].iov_len
            };
            lua_pushinteger( L, iov->used );

            return 2;
        }

        return 1;
    }

    lua_pushnil( L );

    return 1;
}


static int get_lua( lua_State *L )
{
    liovec_t *iov = lauxh_checkudata( L, 1, IOVEC_MT );
    lua_Integer idx = lauxh_checkinteger( L, 2 );

    if( idx >= 0 && idx < iov->used ){
        lauxh_pushref( L, iov->refs[idx] );
    }
    else {
        lua_pushnil( L );
    }

    return 1;
}


static int addn_lua( lua_State *L )
{
    liovec_t *iov = lauxh_checkudata( L, 1, IOVEC_MT );
    lua_Integer n = lauxh_checkinteger( L, 2 );
    lua_Integer nbuf = n / LUAL_BUFFERSIZE;
    lua_Integer remain = n % LUAL_BUFFERSIZE;
    lua_Integer i = 0;
    int used = iov->used + 1;
    luaL_Buffer B;

    lua_settop( L, 0 );
    if( used > IOV_MAX ){
        lua_pushinteger( L, -ENOBUFS );
        return 1;
    }
    else if( iov->nvec < used )
    {
        // increase vec
        void *new = realloc( (void*)iov->data, sizeof( struct iovec ) * used );

        if( !new ){
            lua_pushnil( L );
            lua_pushstring( L, strerror( errno ) );
            return 2;
        }
        iov->nvec = used;
        iov->data = (struct iovec*)new;

        // increase refs
        new = realloc( (void*)iov->refs, sizeof( int ) * used );
        if( !new ){
            lua_pushnil( L );
            lua_pushstring( L, strerror( errno ) );
            return 2;
        }
        iov->refs = (int*)new;
    }

    // create buffer
    luaL_buffinit( L, &B );
    for(; i < nbuf; i++ ){
        luaL_prepbuffer( &B );
        luaL_addsize( &B, LUAL_BUFFERSIZE );
    }
    if( remain ){
        luaL_prepbuffer( &B );
        luaL_addsize( &B, remain );
    }
    luaL_pushresult( &B );

    // maintain result string
    iov->refs[iov->used] = lauxh_ref( L );
    iov->data[iov->used] = (struct iovec){
        .iov_base = (void*)lua_tostring( L, -1 ),
        .iov_len = n
    };
    lua_pushinteger( L, iov->used );

    iov->used = used;

    return 1;
}


static int add_lua( lua_State *L )
{
    liovec_t *iov = lauxh_checkudata( L, 1, IOVEC_MT );
    size_t len = 0;
    const char *str = lauxh_checklstring( L, 2, &len );
    int used = iov->used + 1;

    lua_settop( L, 2 );
    if( used > IOV_MAX ){
        lua_pushinteger( L, -ENOBUFS );
        return 1;
    }
    else if( iov->nvec < used )
    {
        // increase vec
        void *new = realloc( (void*)iov->data, sizeof( struct iovec ) * used );

        if( !new ){
            lua_pushnil( L );
            lua_pushstring( L, strerror( errno ) );
            return 2;
        }
        iov->data = (struct iovec*)new;

        // increase refs
        new = realloc( (void*)iov->refs, sizeof( int ) * used );
        if( !new ){
            lua_pushnil( L );
            lua_pushstring( L, strerror( errno ) );
            return 2;
        }
        iov->refs = (int*)new;

        // update size of vector
        iov->nvec = used;
    }

    iov->refs[iov->used] = lauxh_ref( L );
    iov->data[iov->used] = (struct iovec){
        .iov_base = (void*)str,
        .iov_len = len
    };
    lua_pushinteger( L, iov->used );

    iov->used = used;

    return 1;
}


static int concat_lua( lua_State *L )
{
    liovec_t *iov = lauxh_checkudata( L, 1, IOVEC_MT );

    lua_settop( L, 0 );
    if( iov->used > 0 )
    {
        int i = 0;

        for(; i < iov->used; i++ ){
            lauxh_pushref( L, iov->refs[i] );
        }
        lua_concat( L, iov->used );
    }
    else {
        lua_pushstring( L, "" );
    }

    return 1;
}


static int len_lua( lua_State *L )
{
    liovec_t *iov = lauxh_checkudata( L, 1, IOVEC_MT );

    lua_pushinteger( L, iov->used );

    return 1;
}


static int tostring_lua( lua_State *L )
{
    lua_pushfstring( L, IOVEC_MT ": %p", lua_touserdata( L, 1 ) );
    return 1;
}


static int gc_lua( lua_State *L )
{
    liovec_t *iov = lauxh_checkudata( L, 1, IOVEC_MT );
    int *refs = iov->refs;
    int i = 0;

    free( iov->data );
    for(; i < iov->used; i++ ){
        lauxh_unref( L, refs[i] );
    }
    free( iov->refs );

    return 0;
}


static int new_lua( lua_State *L )
{
    lua_Integer nvec = lauxh_optinteger( L, 1, 0 );

    if( lls_iovec_alloc( L, nvec ) ){
        return 1;
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}


LUALIB_API int luaopen_llsocket_iovec( lua_State *L )
{
    // create metatable
    if( luaL_newmetatable( L, IOVEC_MT ) )
    {
        struct luaL_Reg mmethod[] = {
            { "__gc", gc_lua },
            { "__tostring", tostring_lua },
            { "__len", len_lua },
            { NULL, NULL }
        };
        struct luaL_Reg method[] = {
            { "concat", concat_lua },
            { "add", add_lua },
            { "addn", addn_lua },
            { "get", get_lua },
            { "del", del_lua },
            { NULL, NULL }
        };
        struct luaL_Reg *ptr = mmethod;

        // metamethods
        do {
            lauxh_pushfn2tbl( L, ptr->name, ptr->func );
            ptr++;
        } while( ptr->name );
        // methods
        lua_pushstring( L, "__index" );
        lua_newtable( L );
        ptr = method;
        do {
            lauxh_pushfn2tbl( L, ptr->name, ptr->func );
            ptr++;
        } while( ptr->name );
        lua_rawset( L, -3 );
    }
    lua_pop( L, 1 );

    // create module table
    lua_newtable( L );
    lauxh_pushfn2tbl( L, "new", new_lua );
    lauxh_pushint2tbl( L, "IOV_MAX", IOV_MAX );

    return 1;
}

