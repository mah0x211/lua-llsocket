TARGET=llsocket.$(LIB_EXTENSION)
VARS=$(wildcard var/*.txt)
TMPL=$(wildcard tmpl/*.c)
INSTALL?=install

ifdef LLSOCKET_COVERAGE
COVFLAGS=--coverage
endif

.PHONY: all preprocess install

all: preprocess
	@$(MAKE) $(TARGET)

preprocess:
	lua ./codegen.lua $(VARS) $(TMPL)
	lua ./configure.lua

%.o: %.c
	$(CC) $(CFLAGS) $(WARNINGS) $(COVFLAGS) $(CPPFLAGS) -o $@ -c $<

$(TARGET): $(patsubst %.c,%.o,$(wildcard src/*.c))
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS) $(PLATFORM_LDFLAGS) $(COVFLAGS)

install:
	$(INSTALL) -d $(INST_LIBDIR)
	$(INSTALL) $(TARGET) $(INST_LIBDIR)
	rm -f src/*.o src/*.gcda $(TARGET)

