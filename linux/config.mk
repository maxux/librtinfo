# flags
PREFIX  = /usr/local
LIB	= librtinfo
MAJOR	= 4
VERSION = $(MAJOR).06

CFLAGS  = -fpic -W -Wall -O2 -pipe -ansi -pedantic -std=gnu99 -DVERSION=$(VERSION) -g -I../common
LDFLAGS = -shared -Wl,-soname,$(LIB).so.$(MAJOR)
LIBFILE = $(LIB).so.$(VERSION)

CC = gcc
