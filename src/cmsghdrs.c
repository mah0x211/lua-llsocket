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


static int socket_lua( lua_State *L )
{
    cmsghdrs_t *cmsg = lauxh_checkudata( L, 1, CMSGHDRS_MT );
    int nsock = lua_gettop( L ) - 1;

    // check arguments
    if( nsock )
    {
        if( !lauxh_isnil( L, 2 ) )
        {
            size_t bytes = sizeof( int ) * nsock;
            unsigned char *buf = lua_newuserdata( L, CMSG_SPACE( bytes ) );
            struct cmsghdr *data = (struct cmsghdr*)buf;
            int *ptr = (int*)CMSG_DATA( data );
            int i = 0;

            // set properties
            data->cmsg_len = CMSG_LEN( bytes );
            data->cmsg_level = SOL_SOCKET;
            data->cmsg_type = SCM_RIGHTS;
            // set sockets
            for(; i < nsock; i++ ){
                ptr[i] = lauxh_checkinteger( L, i + 2 );
            }

            // release current data
            if( lauxh_isref( cmsg->ref ) ){
                lauxh_unref( L, cmsg->ref );
            }

            // maintain new data
            cmsg->ref = lauxh_ref( L );
            cmsg->data = data;
        }
        // release current data
        else if( lauxh_isref( cmsg->ref ) ){
            cmsg->ref = lauxh_unref( L, cmsg->ref );
            cmsg->data = NULL;
        }

        return nsock;
    }
    else if( lauxh_isref( cmsg->ref ) )
    {
        // return sockets
        if( cmsg->data->cmsg_level == SOL_SOCKET &&
            cmsg->data->cmsg_type == SCM_RIGHTS )
        {
            int *ptr = (int*)CMSG_DATA( cmsg->data );
            int i = 0;

            nsock = ( cmsg->data->cmsg_len - CMSG_LEN(0) ) / sizeof( int );
            for(; i < nsock; i++ ){
                lua_pushinteger( L, ptr[i] );
            }

            return nsock;
        }
    }

    lua_pushnil( L );

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
            { "socket", socket_lua },
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

