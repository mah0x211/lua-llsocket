/**
 *  Copyright (C) 2014 Masatoshi Teruya
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
 *  llsocket.c
 *  lua-llsocket
 *
 *  Created by Masatoshi Teruya on 14/03/29.
 */

#include "llsocket.h"

LUALIB_API int luaopen_llsocket(lua_State *L)
{
    // register submodule
    luaopen_llsocket_addrinfo(L);

    // create table
    lua_newtable(L);
    // add submodules
    lua_pushstring(L, "socket");
    luaopen_llsocket_socket(L);
    lua_rawset(L, -3);

    lua_pushstring(L, "cmsghdr");
    luaopen_llsocket_cmsghdr(L);
    lua_rawset(L, -3);

    lua_pushstring(L, "cmsghdrs");
    luaopen_llsocket_cmsghdrs(L);
    lua_rawset(L, -3);

    lua_pushstring(L, "msghdr");
    luaopen_llsocket_msghdr(L);
    lua_rawset(L, -3);

    lua_pushstring(L, "inet");
    luaopen_llsocket_inet(L);
    lua_rawset(L, -3);

    lua_pushstring(L, "unix");
    luaopen_llsocket_unix(L);
    lua_rawset(L, -3);

    lua_pushstring(L, "device");
    luaopen_llsocket_device(L);
    lua_rawset(L, -3);

    lua_pushstring(L, "env");
    luaopen_llsocket_env(L);
    lua_rawset(L, -3);

    // for shutdown
    lauxh_pushint2tbl(L, "SHUT_RD", SHUT_RD);
    lauxh_pushint2tbl(L, "SHUT_WR", SHUT_WR);
    lauxh_pushint2tbl(L, "SHUT_RDWR", SHUT_RDWR);

    // for send/recv flags
#define GEN_MSG_FLAGS_DECL
    // address family
#define GEN_ADDRESS_FAMILY_DECL
    // socktype
#define GEN_SOCKTYPE_DECL
    // protocol
#define GEN_IPPROTO_DECL
    // ai_flags
#define GEN_AI_FLAG_DECL
    // ni_flags
#define GEN_NI_FLAG_DECL
    // cmsg_levels
#define GEN_SOL_LEVELS_DECL

    // cmsg_types
#if defined(SCM_CREDENTIALS)
    lauxh_pushint2tbl(L, "SCM_CREDS", SCM_CREDENTIALS);
#elif defined(SCM_CREDS)
    lauxh_pushint2tbl(L, "SCM_CREDS", SCM_CREDS);
#endif

#define GEN_SCM_TYPES_DECL

    return 1;
}
