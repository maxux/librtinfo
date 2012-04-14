EXEC = librtinfo-test

# flags
CFLAGS  = -W -Wall -O2 -pipe -ansi -pedantic -std=gnu99
LDFLAGS = -lrtinfo

CFLAGS  += "-I../"
LDFLAGS += "-L../"
# CC = cc

