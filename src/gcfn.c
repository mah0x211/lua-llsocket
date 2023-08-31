/**
 *  Copyright (C) 2023 Masatoshi Fukunaga
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
 */

#include "llsocket.h"

void lls_gcfn_call(lua_State *L, lls_gcfn_t *gcf)
{
    if (gcf->ref_self != LUA_NOREF) {
        int top = lua_gettop(L);

        // get the each gc function from table
        lauxh_pushref(L, gcf->ref_fn);
        // call gc function
        if (lua_pcall(L, 0, 0, 0) != 0) {
            fprintf(stderr, "failed to call gc function: %s\n",
                    lua_tostring(L, -1));
            lua_pop(L, 1);
        }
        lua_settop(L, top);
        lls_gcfn_del(L, gcf);
    }
}

int lls_gcfn_del(lua_State *L, lls_gcfn_t *gcf)
{
    if (gcf->ref_self == LUA_NOREF) {
        // already deleted
        return 0;
    }

    // detach from list
    if (gcf->prev) {
        gcf->prev->next = gcf->next;
    }
    if (gcf->next) {
        gcf->next->prev = gcf->prev;
    }
    // remove references
    gcf->ref_fn   = lauxh_unref(L, gcf->ref_fn);
    gcf->ref_self = lauxh_unref(L, gcf->ref_self);
    return 1;
}

static int gcfn_closure(lua_State *L)
{
    int argc = 0;

    // get the number of arguments
    lua_pushvalue(L, lua_upvalueindex(1));
    argc = lua_tointeger(L, -1);
    lua_pop(L, 1);
    for (int i = 2; i <= argc; i++) {
        lua_pushvalue(L, lua_upvalueindex(i));
    }
    lua_call(L, argc - 3, (lua_isnoneornil(L, 1)) ? 0 : 1);
    return 0;
}

lls_gcfn_t *lls_gcfn_new(lua_State *L, int argidx)
{
    // number of upvalues that includes the following:
    int nupv        = lua_gettop(L) - argidx + 2;
    lls_gcfn_t *gcf = NULL;

    // check argument type and retain gcfn
    // check a first argument is error function
    if (!lua_isnoneornil(L, argidx)) {
        luaL_checktype(L, argidx, LUA_TFUNCTION);
    }
    // check a second argument is gc function
    luaL_checktype(L, argidx + 1, LUA_TFUNCTION);
    // set number of GC function argument to first upvalue
    lua_pushinteger(L, nupv);
    lua_insert(L, argidx);
    // create closure function
    lua_pushcclosure(L, gcfn_closure, nupv);

    // create new lls_gcfn_t instance
    gcf = lua_newuserdata(L, sizeof(lls_gcfn_t));
    lauxh_setmetatable(L, GCFN_MT);
    // retain self reference
    gcf->ref_self = lauxh_ref(L);
    // retain gc function reference
    gcf->ref_fn   = lauxh_ref(L);
    gcf->prev     = NULL;
    gcf->next     = NULL;

    return gcf;
}

static int tostring_lua(lua_State *L)
{
    lua_pushfstring(L, GCFN_MT ": %p", lua_touserdata(L, 1));
    return 1;
}

void lls_gcfn_init(lua_State *L)
{
    // create metatable
    if (luaL_newmetatable(L, GCFN_MT)) {
        struct luaL_Reg mmethod[] = {
            {"__tostring", tostring_lua},
            {NULL,         NULL        }
        };
        struct luaL_Reg method[] = {
            {NULL, NULL}
        };

        // metamethods
        for (struct luaL_Reg *ptr = mmethod; ptr->name; ptr++) {
            lauxh_pushfn2tbl(L, ptr->name, ptr->func);
        }
        // methods
        lua_newtable(L);
        for (struct luaL_Reg *ptr = method; ptr->name; ptr++) {
            lauxh_pushfn2tbl(L, ptr->name, ptr->func);
        }
        lua_setfield(L, -2, "__index");
    }
    lua_pop(L, 1);
}
