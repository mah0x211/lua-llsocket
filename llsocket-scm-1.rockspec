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
    "lua >= 5.1"
}
build = {
    type = "builtin",
    modules = {
        llsocket = {
            sources = { 
                "src/llsocket.c",
                "src/inet.c",
            },
        }
    }
}

