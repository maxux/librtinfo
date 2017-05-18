include config.mk

SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: $(LIBFILE)

$(LIBFILE): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
	ar rcs $(LIB).a $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -fv *.o

debug: CFLAGS += -DLIBRTINFO_DEBUG=1 -g
debug: $(LIBFILE)

mrproper: clean
	rm -fv $(LIBFILE)
	rm -fv $(LIB).a

install:
	mkdir -p $(DESTDIR)$(PREFIX)$(LIBDIR)
	cp -f $(LIB).so* $(DESTDIR)$(PREFIX)$(LIBDIR)
	ln -sf $(LIBFILE) $(DESTDIR)$(PREFIX)$(LIBDIR)/$(LIB).so.$(MAJOR)
	ln -sf $(LIB).so.$(MAJOR) $(DESTDIR)$(PREFIX)$(LIBDIR)/$(LIB).so
	cp -f *.a $(DESTDIR)$(PREFIX)$(LIBDIR)
	mkdir -p $(DESTDIR)$(PREFIX)/include
	cp rtinfo.h $(DESTDIR)$(PREFIX)/include/
	@echo -e "\nYou may want to run ldconfig if you install this locally"

uninstall:
	rm -f $(DESTDIR)$(PREFIX)$(LIBDIR)/$(LIB).*
	rm -f $(DESTDIR)$(PREFIX)/include/rtinfo.h
