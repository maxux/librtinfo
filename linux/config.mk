PREFIX = /usr/local/
LIBDIR = lib
LIB	= librtinfo
MAJOR = 4
VERSION = $(MAJOR).11

CFLAGS += -fpic -W -Wall -O2 -pipe -ansi -pedantic -std=gnu99 -DVERSION=$(VERSION)
LDFLAGS += -shared -Wl,-soname,$(LIB).so.$(MAJOR)
LIBFILE = $(LIB).so.$(VERSION)
