EXEC = librtinfo-test

# flags
CFLAGS  = -W -Wall -O2 -pipe -ansi -pedantic -std=c99
LDFLAGS = -lrtinfo

CFLAGS  += "-I../"
LDFLAGS += "-L../"
# CC = cc

