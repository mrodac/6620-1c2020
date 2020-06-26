CFLAGS = -O0 -Wall -Werror -pedantic -pedantic-errors

SRC := src
SOURCES := $(SRC)/main.c $(SRC)/cache.c

all: cache 

cache: 
	$(CC) $(SOURCES) -o cache $(CFLAGS)

clean:
	$(RM) -f cache 