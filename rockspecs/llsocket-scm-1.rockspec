package = "llsocket"
version = "scm-1"
source = {
    url = "git://github.com/mah0x211/lua-llsocket.git"
}
description = {
    summary = "low-level socket module",
    homepage = "https://github.com/mah0x211/lua-llsocket",
    license = "MIT/X11",
    maintainer = "Masatoshi Teruya"
}
dependencies = {
    "lua >= 5.1",
    "util >= 1.4.1"
}
build = {
    type = "command",
    build_command = [[
        	autoreconf -ivf && CFLAGS="$(CFLAGS)" CPPFLAGS="-I$(LUA_INCDIR)" LIBFLAG="$(LIBFLAG)" OBJ_EXTENSION="$(OBJ_EXTENSION)" LIB_EXTENSION="$(LIB_EXTENSION)" LIBDIR="$(LIBDIR)" TMPLDIR="tmpl" VARDIR="var" ./configure && make clean && make
    ]],
    install_command = [[
        make install
    ]]
}
