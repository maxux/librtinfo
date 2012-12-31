include config.mk

SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: $(LIBFILE)

$(LIBFILE): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	del /f *.o

mrproper: clean
	del /f $(LIBFILE) $(LIB).*dll

install:
	# mkdir -p $(PREFIX)/lib
	# cp -Pf *.dylib* $(PREFIX)/lib/
	# mkdir -p $(PREFIX)/include
	# cp -f rtinfo.h $(PREFIX)/include/

uninstall:
	# rm -f $(PREFIX)/lib/$(LIB).*
	# rm -f $(PREFIX)/include/rtinfo.h
