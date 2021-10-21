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
 *  msghdr.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 17/08/15.
 */

#include "llsocket.h"

static int flags_lua(lua_State *L)
{
    lmsghdr_t *msg = lauxh_checkudata(L, 1, MSGHDR_MT);

    lua_pushinteger(L, msg->flags);

    return 1;
}

static int control_lua(lua_State *L)
{
    lmsghdr_t *msg = lauxh_checkudata(L, 1, MSGHDR_MT);

    // push ref
    lauxh_pushref(L, msg->control_ref);

    if (lua_gettop(L) > 1) {
        // check argument
        cmsghdrs_t *cmsgs = lauxh_optudata(L, 2, CMSGHDRS_MT, NULL);

        // release current ref
        msg->control_ref = lauxh_unref(L, msg->control_ref);
        msg->control     = NULL;
        if (cmsgs) {
            msg->control_ref = lauxh_refat(L, 2);
            msg->control     = cmsgs;
        }
    }

    return 1;
}

static int iov_lua(lua_State *L)
{
    lmsghdr_t *msg = lauxh_checkudata(L, 1, MSGHDR_MT);

    // push ref
    lauxh_pushref(L, msg->iov_ref);

    if (lua_gettop(L) > 1) {
        // check argument
        lua_iovec_t *iov = lauxh_optudata(L, 2, IOVEC_MT, NULL);

        // release current ref
        msg->iov_ref = lauxh_unref(L, msg->iov_ref);
        msg->name    = NULL;
        if (iov) {
            msg->iov_ref = lauxh_refat(L, 2);
            msg->iov     = iov;
        }
    }

    return 1;
}

static int name_lua(lua_State *L)
{
    lmsghdr_t *msg = lauxh_checkudata(L, 1, MSGHDR_MT);

    // push ref
    lauxh_pushref(L, msg->name_ref);

    if (lua_gettop(L) > 1) {
        struct addrinfo *info = lauxh_optudata(L, 2, ADDRINFO_MT, NULL);

        // release current ref
        msg->name_ref = lauxh_unref(L, msg->name_ref);
        msg->name     = NULL;
        if (info) {
            msg->name_ref = lauxh_refat(L, 2);
            msg->name     = info;
        }
    }

    return 1;
}

static int tostring_lua(lua_State *L)
{
    lua_pushfstring(L, MSGHDR_MT ": %p", lua_touserdata(L, 1));
    return 1;
}

static int gc_lua(lua_State *L)
{
    lmsghdr_t *msg = lauxh_checkudata(L, 1, MSGHDR_MT);

    lauxh_unref(L, msg->name_ref);
    lauxh_unref(L, msg->iov_ref);
    lauxh_unref(L, msg->control_ref);

    return 0;
}

static int new_lua(lua_State *L)
{
    lmsghdr_t *msg = lua_newuserdata(L, sizeof(lmsghdr_t));

    *msg = (lmsghdr_t){.name_ref    = LUA_NOREF,
                       .iov_ref     = LUA_NOREF,
                       .control_ref = LUA_NOREF,
                       .flags       = 0,
                       .name        = NULL,
                       .iov         = NULL,
                       .control     = NULL};
    lauxh_setmetatable(L, MSGHDR_MT);

    return 1;
}

LUALIB_API int luaopen_llsocket_msghdr(lua_State *L)
{
    // create metatable
    if (luaL_newmetatable(L, MSGHDR_MT)) {
        struct luaL_Reg mmethod[] = {
            {"__gc",       gc_lua      },
            {"__tostring", tostring_lua},
            {NULL,         NULL        }
        };
        struct luaL_Reg method[] = {
            {"name",    name_lua   },
            {"iov",     iov_lua    },
            {"control", control_lua},
            {"flags",   flags_lua  },
            {NULL,      NULL       }
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

    return 1;
}
