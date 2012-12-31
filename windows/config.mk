# flags
PREFIX  = /usr/local
LIB	= librtinfo
MAJOR	= 4
VERSION = $(MAJOR).01

CFLAGS  = -W -Wall -O2 -pipe -ansi -pedantic -std=c99 -DVERSION=$(VERSION) -g -I../common
LDFLAGS = -shared -Wl,--out-implib,$(LIB).a -o $(LIB).dll
LIBFILE = $(LIB).$(VERSION).dll

CC = gcc
