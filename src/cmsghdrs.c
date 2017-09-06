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
 *  cmsghdrs.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 17/08/16.
 */

#include "llsocket.h"


static int pop_lua( lua_State *L )
{
    cmsghdrs_t *cmsg = lauxh_checkudata( L, 1, CMSGHDRS_MT );
    struct msghdr msg = {
        .msg_name = NULL,
        .msg_namelen = 0,
        .msg_iov = NULL,
        .msg_iovlen = 0,
        .msg_control = cmsg->data,
        .msg_controllen = cmsg->len,
        .msg_flags = 0
    };
    struct cmsghdr *item = CMSG_FIRSTHDR( &msg );

    if( item )
    {
        size_t len = item->cmsg_len - CMSG_LEN(0);

        lua_settop( L, 0 );
        // create cmsghdr
        lua_pushlstring( L, (void*)CMSG_DATA( item ), len );
        if( lls_cmsghdr_alloc( L, item->cmsg_level, item->cmsg_type ) )
        {
            // remove first header
            cmsg->len -= CMSG_SPACE( len );
            if( cmsg->len ){
                memmove( cmsg->data, cmsg->data + CMSG_SPACE( len ), cmsg->len );
            }

            return 1;
        }

        // got error
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );

        return 2;
    }

    // not found
    lua_pushnil( L );

    return 1;
}


static int push_lua( lua_State *L )
{
    cmsghdrs_t *cmsg = lauxh_checkudata( L, 1, CMSGHDRS_MT );
    cmsghdr_t *item = lauxh_checkudata( L, 2, CMSGHDR_MT );
    char *data = cmsg->data;
    size_t len = cmsg->len + CMSG_SPACE( item->len );

    if( len > cmsg->bytes )
    {
        if( ( data = lua_newuserdata( L, len ) ) ){
            memset( data, 0, len );
            // copy current and new data
            memcpy( data, cmsg->data, cmsg->len );

            // release old data
            lauxh_unref( L, cmsg->ref );
            cmsg->ref = lauxh_ref( L );
            cmsg->data = data;
            cmsg->bytes = len;
        }
        // got error
        else {
            lua_pushboolean( L, 0 );
            lua_pushstring( L, strerror( errno ) );

            return 2;
        }
    }

    // set properties
    *(struct cmsghdr*)(data + cmsg->len) = (struct cmsghdr){
        .cmsg_level = item->level,
        .cmsg_type = item->type,
        .cmsg_len = CMSG_LEN( item->len )
    };
    memcpy( data + cmsg->len + CMSG_LEN(0), item->data, item->len );
    cmsg->len = len;

    lua_pushboolean( L, 1 );

    return 1;
}


static int tostring_lua( lua_State *L )
{
    lua_pushfstring( L, CMSGHDRS_MT ": %p", lua_touserdata( L, 1 ) );
    return 1;
}


static int gc_lua( lua_State *L )
{
    cmsghdrs_t *cmsg = lauxh_checkudata( L, 1, CMSGHDRS_MT );

    if( lauxh_isref( cmsg->ref ) ){
        lauxh_unref( L, cmsg->ref );
    }

    return 0;
}


static int new_lua( lua_State *L )
{
    cmsghdrs_t *cmsg = lua_newuserdata( L, sizeof( cmsghdrs_t ) );

    if( cmsg ){
        cmsg->ref = LUA_NOREF;
        cmsg->data = NULL;
        cmsg->len = 0;
        cmsg->bytes = 0;
        lauxh_setmetatable( L, CMSGHDRS_MT );
        return 1;
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}


LUALIB_API int luaopen_llsocket_cmsghdrs( lua_State *L )
{
    // create metatable
    if( luaL_newmetatable( L, CMSGHDRS_MT ) )
    {
        struct luaL_Reg mmethod[] = {
            { "__gc", gc_lua },
            { "__tostring", tostring_lua },
            { NULL, NULL }
        };
        struct luaL_Reg method[] = {
            { "push", push_lua },
            { "pop", pop_lua },
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

