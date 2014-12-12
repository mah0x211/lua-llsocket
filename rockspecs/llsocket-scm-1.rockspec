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
    type = "make",
    build_variables = {
        PACKAGE         = "llsocket",
        SRCDIR          = "src",
        TMPLDIR         = "tmpl",
        VARDIR          = "var",
        CFLAGS          = "$(CFLAGS)",
        WARNINGS        = "-Wall -Wno-trigraphs -Wmissing-field-initializers -Wreturn-type -Wmissing-braces -Wparentheses -Wno-switch -Wunused-function -Wunused-label -Wunused-parameter -Wunused-variable -Wunused-value -Wuninitialized -Wunknown-pragmas -Wshadow -Wsign-compare",
        CPPFLAGS        = "-I$(LUA_INCDIR)",
        LDFLAGS         = "$(LIBFLAG)",
        LIB_EXTENSION   = "$(LIB_EXTENSION)"
    },
    install_variables = {
        PACKAGE         = "llsocket",
        SRCDIR          = "src",
        LIBDIR          = "$(LIBDIR)",
        PREFIX          = "$(PREFIX)",
        LIB_EXTENSION   = "$(LIB_EXTENSION)"
    }
}

