
CC=gcc
CFLAGS=-ansi -pedantic
LDFLAGS=
EXEC=server client
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

all: $(EXEC)
	
server:
	$(CC) -o $@ $@.c
client:
	$(CC) -o $@ $@.c

.PHONY: clean

clean:
	@rm -rf *.o
	@rm -rf server
	@rm -rf client
