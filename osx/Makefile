include config.mk

SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: $(LIBFILE)

$(LIBFILE): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
	ln -sf $(LIB).dylib.$(VERSION) $(LIB).dylib


%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -fv *.o

mrproper: clean
	rm -fv $(LIBFILE) $(LIB).dylib*

install:
	mkdir -p $(PREFIX)/lib
	cp -Pf *.dylib* $(PREFIX)/lib/
	mkdir -p $(PREFIX)/include
	cp -f rtinfo.h $(PREFIX)/include/

uninstall:
	rm -f $(PREFIX)/lib/$(LIB).*
	rm -f $(PREFIX)/include/rtinfo.h
