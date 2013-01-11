include config.mk

SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

ifdef SystemRoot
	RM   = del /Q
	EXEC := $(EXEC).exe
else
	RM  = rm -fv
endif

all:
	@echo "Please specify: linux windows osx"

linux: trg_linux
trg_linux: LDFLAGS += -L../linux
trg_linux: options $(EXEC)

windows: options trg_windows $(EXEC)
trg_windows: LDFLAGS += -L../windows
trg_windows: options $(EXEC)

osx: options trg_osx $(EXEC)
trg_osx: LDFLAGS += -L../osx
trg_osx: options $(EXEC)

options: config.mk
	@echo $(EXEC) build options:
	@echo "CFLAGS   = $(CFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"
	@echo "CC       = $(CC)"

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	$(RM) *.o

mrproper: clean
	$(RM) $(EXEC)
