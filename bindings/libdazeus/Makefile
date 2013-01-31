CFLAGS = -Wall -Iinclude -Icontrib/jansson/src $(EXTRACFLAGS)
JANSSON_CFLAGS=-Icontrib/jansson/src

JANSSON_FILES = contrib/jansson/src/dump.c \
	contrib/jansson/src/error.c \
	contrib/jansson/src/hashtable.c \
	contrib/jansson/src/load.c \
	contrib/jansson/src/memory.c \
	contrib/jansson/src/pack_unpack.c \
	contrib/jansson/src/strbuffer.c \
	contrib/jansson/src/strconv.c \
	contrib/jansson/src/utf.c \
	contrib/jansson/src/value.c

JANSSON_OBJECTS = $(JANSSON_FILES:.c=.o)

lib/libdazeus.a: lib/libdazeus.o $(JANSSON_OBJECTS)
	ar cur lib/libdazeus.a lib/libdazeus.o $(JANSSON_OBJECTS)

contrib/jansson/src/%.o: $(@:.o=.c) contrib/jansson/src/jansson_config.h
	$(CC) -c -o $@ $(@:.o=.c) $(JANSSON_CFLAGS)

contrib/jansson/src/jansson_config.h: jansson_config.h
	cp jansson_config.h $@

lib/libdazeus.o: src/libdazeus.c include/libdazeus.h contrib/jansson/src/jansson_config.h
	mkdir -p lib
	$(CC) -c -o lib/libdazeus.o src/libdazeus.c $(CFLAGS)

examples/networks: examples/networks.c include/libdazeus.h lib/libdazeus.a
	$(CXX) -o examples/networks examples/networks.c lib/libdazeus.a $(CFLAGS)

examples/counter: examples/counter.c include/libdazeus.h lib/libdazeus.a
	$(CXX) -o examples/counter examples/counter.c lib/libdazeus.a $(CFLAGS)

examples/counterreset: examples/counterreset.c include/libdazeus.h lib/libdazeus.a
	$(CXX) -o examples/counterreset examples/counterreset.c lib/libdazeus.a $(CFLAGS)

examples/monitor: examples/monitor.c include/libdazeus.h lib/libdazeus.a
	$(CXX) -o examples/monitor examples/monitor.c lib/libdazeus.a $(CFLAGS)

examples/scope: examples/scope.c include/libdazeus.h lib/libdazeus.a
	$(CXX) -o examples/scope examples/scope.c lib/libdazeus.a $(CFLAGS)

.PHONY : clean distclean
clean:
	rm -f lib/libdazeus.o
	rm -f $(JANSSON_OBJECTS)
	rm -rf examples/networks.dSYM
	rm -rf examples/counter.dSYM
	rm -rf examples/counterreset.dSYM
	rm -rf examples/monitor.dSYM
	rm -rf examples/scope.dSYM

distclean: clean
	rm -f lib/libdazeus.a
	rm -f examples/networks examples/counter examples/counterreset
	rm -f examples/monitor examples/scope
