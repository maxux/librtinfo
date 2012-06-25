# flags
PREFIX  = /usr/local
LIB	= librtinfo
MAJOR	= 3
VERSION = $(MAJOR).42

CFLAGS  = -fpic -W -Wall -O2 -pipe -ansi -pedantic -std=gnu99 -DVERSION=$(VERSION)
LDFLAGS = -shared -Wl,-soname,$(LIB).so.$(MAJOR)
LIBFILE = $(LIB).so.$(VERSION)

# CC = cc

