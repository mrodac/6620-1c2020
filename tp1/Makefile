CFLAGS = -O0 -Wall -Werror -pedantic -pedantic-errors

SRC := src
machine := $(shell uname -m)

ifeq ($(machine),mips64)
	SOURCES := $(SRC)/main.c $(SRC)/vecinos.S
else
	SOURCES := $(SRC)/main.c $(SRC)/vecinos.c
endif

all: conway 

conway: 
	$(CC) $(SOURCES) -o conway -lm $(CFLAGS)

clean:
	$(RM) -f conway *.pbm *.mp4