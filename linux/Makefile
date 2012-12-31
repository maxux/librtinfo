include config.mk

SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: $(LIBFILE)

$(LIBFILE): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
	/sbin/ldconfig -n .
	ln -sf $(LIB).so.$(MAJOR) $(LIB).so

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -fv *.o

mrproper: clean
	rm -fv $(LIBFILE) $(LIB).so*

install:
	mkdir -p $(PREFIX)/lib
	cp -Pf *.so* $(PREFIX)/lib/
	ldconfig
	mkdir -p $(PREFIX)/include
	cp -f rtinfo.h $(PREFIX)/include/

uninstall:
	rm -f $(PREFIX)/lib/$(LIB).*
	rm -f $(PREFIX)/include/rtinfo.h
