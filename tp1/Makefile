CFLAGS = -O0 -Wall -Werror -pedantic -pedantic-errors

SRC := src
SOURCES := $(SRC)/main.c $(SRC)/vecinos.c

ifndef C_IMPL
	ifeq ($(machine),mips64)
		SOURCES := $(SRC)/main.c $(SRC)/vecinos.S
	endif
endif	

all: conway

conway: 
	$(CC) $(SOURCES) -o conway -lm $(CFLAGS)

clean:
	$(RM) -f conway *.pbm