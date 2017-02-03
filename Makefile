
CC=gcc
CFLAGS= -ansi -pedantic
LDFLAGS=
EXEC= main
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

all: $(EXEC)
	
$(EXEC):
	$(CC) -o $@ $@.c $@.h

.PHONY: clean

clean:
	@rm -rf *.o
	@rm -rf $(EXEC)
	
