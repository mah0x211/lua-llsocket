/**
 *  Copyright (C) 2017 Masatoshi Teruya
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
 *  cmsghdr.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 17/09/05.
 */

#include "llsocket.h"

static int data_lua(lua_State *L)
{
    lls_cmsghdr_t *cmsg = lauxh_checkudata(L, 1, CMSGHDR_MT);

    lauxh_pushref(L, cmsg->ref);
    if (cmsg->level == SOL_SOCKET && cmsg->type == SCM_RIGHTS) {
        size_t len       = 0;
        const char *data = lauxh_checklstring(L, -1, &len);
        int nfd          = 0;

        if (!len) {
            lua_pushnil(L);
            return 1;
        }

        // SCM_RIGHTS data must be a multiple of sizeof(int32_t)
        if (len % sizeof(int32_t) != 0) {
            return luaL_error(L,
                              "SCM_RIGHTS protocol violation: "
                              "data length is not a multiple of %zu",
                              sizeof(int32_t));
        }

        nfd = len / sizeof(int);
        for (int i = 0; i < nfd; i++) {
            int32_t fd;
            memcpy(&fd, data + i * sizeof(int32_t), sizeof(int32_t));
            lua_pushinteger(L, fd);
        }
        return nfd;
    }

    return 1;
}

static int type_lua(lua_State *L)
{
    lls_cmsghdr_t *cmsg = lauxh_checkudata(L, 1, CMSGHDR_MT);

    lua_pushinteger(L, cmsg->type);

    return 1;
}

static int level_lua(lua_State *L)
{
    lls_cmsghdr_t *cmsg = lauxh_checkudata(L, 1, CMSGHDR_MT);

    lua_pushinteger(L, cmsg->level);

    return 1;
}

static int len_lua(lua_State *L)
{
    lls_cmsghdr_t *cmsg = lauxh_checkudata(L, 1, CMSGHDR_MT);

    lua_pushinteger(L, cmsg->len);

    return 1;
}

static int tostring_lua(lua_State *L)
{
    lua_pushfstring(L, CMSGHDR_MT ": %p", lua_touserdata(L, 1));
    return 1;
}

static int gc_lua(lua_State *L)
{
    lls_cmsghdr_t *cmsg = lauxh_checkudata(L, 1, CMSGHDR_MT);

    if (lauxh_isref(cmsg->ref)) {
        lauxh_unref(L, cmsg->ref);
    }

    return 0;
}

static int new_lua(lua_State *L)
{
    int level = lauxh_checkinteger(L, 1);
    int type  = lauxh_checkinteger(L, 2);

    lua_settop(L, 3);
    lls_cmsghdr_alloc(L, level, type);

    return 1;
}

static int rights_lua(lua_State *L)
{
    int argc = lua_gettop(L);

    if (argc) {
        // check arguments
        for (int i = 1; i <= argc; i++) {
            int32_t fd = lauxh_checkint32(L, i);
            lua_pushlstring(L, (const char *)&fd, sizeof(fd));
        }
        lua_concat(L, argc);
        lua_replace(L, 1);
        lua_settop(L, 1);
        lls_cmsghdr_alloc(L, SOL_SOCKET, SCM_RIGHTS);

        return 1;
    }

    lua_pushnil(L);

    return 1;
}

LUALIB_API int luaopen_llsocket_cmsghdr(lua_State *L)
{
    // create metatable
    if (luaL_newmetatable(L, CMSGHDR_MT)) {
        struct luaL_Reg mmethod[] = {
            {"__gc",       gc_lua      },
            {"__tostring", tostring_lua},
            {"__len",      len_lua     },
            {NULL,         NULL        }
        };
        struct luaL_Reg method[] = {
            {"level", level_lua},
            {"type",  type_lua },
            {"data",  data_lua },
            {NULL,    NULL     }
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

    // create module table
    lua_newtable(L);
    lauxh_pushfn2tbl(L, "new", new_lua);
    lauxh_pushfn2tbl(L, "rights", rights_lua);

    return 1;
}
