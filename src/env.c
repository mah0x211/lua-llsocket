/**
 *  Copyright (C) 2021 Masatoshi Fukunaga
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 */

#include "llsocket.h"

LUALIB_API int luaopen_llsocket_env(lua_State *L)
{
    // create table
    lua_newtable(L);

    lua_pushliteral(L, "os");
    lua_pushliteral(L,
#if defined(__APPLE__)
                    "apple"
#elif defined(__DragonFly__)
                    "dragonfly"
#elif defined(__FreeBSD__)
                    "freebsd"
#elif defined(__NetBSD__)
                    "netbsd"
#elif defined(__OpenBSD__)
                    "openbsd"
#elif defined(__linux__)
                    "linux"
#else
                    "unknown"
#endif
    );
    lua_rawset(L, -3);

    lua_pushliteral(L, "unixpath_max");
    lua_pushinteger(L, UNIXPATH_MAX);
    lua_rawset(L, -3);

    lua_pushliteral(L, "somaxconn");
    lua_pushinteger(L, SOMAXCONN);
    lua_rawset(L, -3);

    return 1;
}
