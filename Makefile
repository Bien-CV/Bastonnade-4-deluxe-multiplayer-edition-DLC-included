
CC=g++
CFLAGS=
LDFLAGS=
EXEC= serverPrototype
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

all: $(EXEC)
	
$(EXEC):
	$(CC) -o $@ $@.c

.PHONY: clean

clean:
	@rm -rf *.o
	@rm -rf $(EXEC)
	
