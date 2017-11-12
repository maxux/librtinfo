EXEC = librtinfo-test

# flags
CFLAGS  = -W -Wall -O2 -pipe -ansi -pedantic -std=c99 -I../linux
LDFLAGS = -lrtinfo -L../linux
