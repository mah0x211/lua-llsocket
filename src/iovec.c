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
        iov->bytes -= iov->lens[idx];
        iov->used--;

        // swap index
        // used: 6
        //  del: 4: -> 0, 1, 2, 3, '4', 5
        //          -> 0, 1, 2, 3, [4], 5
        //          -> 0, 1, 2, 3, [5]
        // used: 5
        //  del: 2: -> 0, 1, '2', 3, 5
        //          -> 0, 1, [3], 3, 5
        //          -> 0, 1, 3, [5]
        // used: 4
        //  del: 0: -> '0', 1, 3, 5
        //          -> [3], 1, 3, 5
        //          -> 3, 1, [5]
        // used: 3
        //  del: 2: -> 3, 1, '5'
        //          -> 3, 1
        if( iov->used != idx ){
            lua_pushinteger( L, iov->used - 1 );
            // fill holes in array
            iov->refs[idx] = iov->refs[iov->used - 1];
            iov->lens[idx] = iov->lens[iov->used - 1];
            iov->data[idx] = iov->data[iov->used - 1];
            // move last data
            iov->refs[iov->used - 1] = iov->refs[iov->used];
            iov->lens[iov->used - 1] = iov->lens[iov->used];
            iov->data[iov->used - 1] = iov->data[iov->used];
            return 2;
        }

        return 1;
    }

    lua_pushnil( L );

    return 1;
}


static inline int pushstr( lua_State *L, liovec_t *iov, int idx )
{
    // copy string if actual length is not equal to allocated size
    if( iov->data[idx].iov_len - iov->lens[idx] ){
        lua_pushlstring( L, iov->data[idx].iov_base, iov->lens[idx] );
        return 1;
    }

    lauxh_pushref( L, iov->refs[idx] );
    return 0;
}


static int get_lua( lua_State *L )
{
    liovec_t *iov = lauxh_checkudata( L, 1, IOVEC_MT );
    lua_Integer idx = lauxh_checkinteger( L, 2 );

    if( idx >= 0 && idx < iov->used ){
        pushstr( L, iov, idx );
    }
    else {
        lua_pushnil( L );
    }

    return 1;
}


static inline int addstr_lua( lua_State *L, liovec_t *iov )
{
    size_t len = 0;
    const char *str = lauxh_checklstring( L, 2, &len );
    int used = iov->used + 1;

    lua_settop( L, 2 );
    if( used > IOV_MAX ){
        errno = ENOBUFS;
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        return 2;
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

        // increase lens
        new = realloc( (void*)iov->lens, sizeof( size_t ) * used );
        if( !new ){
            lua_pushnil( L );
            lua_pushstring( L, strerror( errno ) );
            return 2;
        }
        iov->lens = (size_t*)new;

        // update size of vector
        iov->nvec = used;
    }

    // maintain string
    iov->refs[iov->used] = lauxh_ref( L );
    iov->lens[iov->used] = len;
    iov->data[iov->used] = (struct iovec){
        .iov_base = (void*)str,
        .iov_len = len
    };
    lua_pushinteger( L, iov->used );

    iov->used = used;
    iov->bytes += len;

    return 1;
}


static int addn_lua( lua_State *L )
{
    liovec_t *iov = lauxh_checkudata( L, 1, IOVEC_MT );
    lua_Integer n = lauxh_checkinteger( L, 2 );

    // check argument
    lauxh_argcheck(
        L, n > 0, 2, "1 or more integer expected, got less than 1"
    );

    lua_settop( L, 1 );
    // create buffer
    lauxh_pushbuffer( L, n );

    return addstr_lua( L, iov );
}


static int add_lua( lua_State *L )
{
    liovec_t *iov = lauxh_checkudata( L, 1, IOVEC_MT );

    return addstr_lua( L, iov );
}


static int concat_lua( lua_State *L )
{
    liovec_t *iov = lauxh_checkudata( L, 1, IOVEC_MT );

    lua_settop( L, 0 );
    if( iov->used > 0 )
    {
        int used = iov->used;
        int i = 0;

        // push actual used values
        while( i < used && pushstr( L, iov, i++ ) == 0 ){}
        lua_concat( L, i );
    }
    else {
        lua_pushstring( L, "" );
    }

    return 1;
}


static int consume_lua( lua_State *L )
{
    liovec_t *iov = lauxh_checkudata( L, 1, IOVEC_MT );
    lua_Integer n = lauxh_checkinteger( L, 2 );

    // check argument
    lauxh_argcheck(
        L, n >= 0, 2, "unsigned integer expected, got signed integer"
    );

    if( (size_t)n >= iov->bytes )
    {
        int *refs = iov->refs;
        int used = iov->used;
        int i = 0;

        for(; i < used; i++ ){
            lauxh_unref( L, refs[i] );
        }
        iov->used = 0;
        iov->bytes = 0;
    }
    else if( n )
    {
        struct iovec *data = iov->data;
        int *refs = iov->refs;
        size_t *lens = iov->lens;
        int used = iov->used;
        int head = 0;
        size_t len = 0;

        iov->bytes -= n;

        while( n > 0 )
        {
            len = lens[head];
            // update the last data block
            if( len > (size_t)n ){
                char *ptr = data[head].iov_base;

                len -= n;
                memmove( ptr, ptr + n, len );
                ptr[len] = 0;
                lens[head] = len;
                break;
            }

            // remove refenrece
            n -= lens[head];
            lauxh_unref( L, refs[head] );
            head++;
        }

        iov->used = used - head;
        // update references
        if( head && head != used )
        {
            int i = 0;

            for(; head < used; head++ ){
                data[i] = data[head];
                refs[i] = refs[head];
                lens[i] = lens[head];
                i++;
            }
        }
    }

    lua_pushinteger( L, iov->bytes );
    return 1;
}


static int bytes_lua( lua_State *L )
{
    liovec_t *iov = lauxh_checkudata( L, 1, IOVEC_MT );

    lua_pushinteger( L, iov->bytes );

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
    int used = iov->used;
    int i = 0;

    free( iov->lens );
    free( iov->data );
    for(; i < used; i++ ){
        lauxh_unref( L, refs[i] );
    }
    free( refs );

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
            { "bytes", bytes_lua },
            { "consume", consume_lua },
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

    return 1;
}

