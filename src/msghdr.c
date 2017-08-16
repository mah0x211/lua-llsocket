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
 *  msghdr.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 17/08/15.
 */

#include "llsocket.h"


static int name_lua( lua_State *L )
{
    lmsghdr_t *msg = lauxh_checkudata( L, 1, MSGHDR_MT );

    if( lua_gettop( L ) > 1 )
    {
        // release current ref
        msg->name_ref = lauxh_unref( L, msg->name_ref );
        // check argument
        lua_settop( L, 2 );
        if( !lauxh_isnil( L, 2 ) ){
            lauxh_checkudata( L, 2, ADDRINFO_MT );
            msg->name_ref = lauxh_ref( L );
        }
    }

    // push ref
    lauxh_pushref( L, msg->name_ref );

    return 1;
}


static int flags_lua( lua_State *L )
{
    lmsghdr_t *msg = lauxh_checkudata( L, 1, MSGHDR_MT );

    lua_pushinteger( L, msg->flags );

    return 1;
}


static int iov_lua( lua_State *L )
{
    lmsghdr_t *msg = lauxh_checkudata( L, 1, MSGHDR_MT );

    lauxh_pushref( L, msg->iov_ref );

    return 1;
}


static int tostring_lua( lua_State *L )
{
    lua_pushfstring( L, MSGHDR_MT ": %p", lua_touserdata( L, 1 ) );
    return 1;
}


static int gc_lua( lua_State *L )
{
    lmsghdr_t *msg = lauxh_checkudata( L, 1, MSGHDR_MT );

    lauxh_unref( L, msg->iov_ref );

    return 0;
}


static int new_lua( lua_State *L )
{
    lmsghdr_t *msg = NULL;
    liovec_t *iov = NULL;
    lua_Integer nvec = 0;
    int ref = LUA_NOREF;

    // check argument
    if( lua_gettop( L ) > 0 )
    {
        lua_settop( L, 1 );
        switch( lua_type( L, 1 ) ){
            case LUA_TUSERDATA:
                iov = lauxh_checkudata( L, 1, IOVEC_MT );
                ref = lauxh_ref( L );
                goto ALLOC_MSGHDR;

            default:
                nvec = lauxh_optinteger( L, 1, 0 );
        }
    }

    // alloc iovec
    if( !( iov = lls_iovec_alloc( L, nvec ) ) ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        return 2;
    }
    ref = lauxh_ref( L );

ALLOC_MSGHDR:
    msg = lua_newuserdata( L, sizeof( lmsghdr_t ) );
    if( msg ){
        *msg = (lmsghdr_t){
            .name_ref = LUA_NOREF,
            .iov_ref = ref,
            .control_ref = LUA_NOREF,
            .flags = 0,
            .iov = iov
        };

        lauxh_setmetatable( L, MSGHDR_MT );
        return 1;
    }



    // got error
    lauxh_unref( L, ref );
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}


LUALIB_API int luaopen_llsocket_msghdr( lua_State *L )
{
    // create metatable
    if( luaL_newmetatable( L, MSGHDR_MT ) )
    {
        struct luaL_Reg mmethod[] = {
            { "__gc", gc_lua },
            { "__tostring", tostring_lua },
            { NULL, NULL }
        };
        struct luaL_Reg method[] = {
            { "name", name_lua },
            { "iov", iov_lua },
            { "flags", flags_lua },
            { NULL, NULL }
        };
        struct luaL_Reg *ptr = mmethod;

        luaopen_llsocket_iovec( L );
        lua_pop( L, 1 );

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

