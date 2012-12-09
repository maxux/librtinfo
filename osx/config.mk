# flags
PREFIX  = /usr/local
LIB	= librtinfo
MAJOR	= 4
VERSION = $(MAJOR).01

CFLAGS  = -fpic -W -Wall -O2 -pipe -ansi -pedantic -std=gnu99 -DVERSION=$(VERSION) -g
LDFLAGS = -dynamiclib -Wl,-install_name,$(LIB).dylib
LIBFILE = $(LIB).dylib.$(VERSION)

CC = gcc
