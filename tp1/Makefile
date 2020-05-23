# Opciones para el compilador C/C++ para tratamiento de errores y warnings.
CFLAGS = -O0 -Wall -Werror -pedantic -pedantic-errors

machine := $(shell uname -m)

.PHONY: all
SRC := src
OBJ := obj

ifeq ($(uname_p),mips64)
	SOURCES := $(wildcard $(SRC)/*.c *.s)
	OBJECTS := $(patsubst $(SRC)/%.c %.s, $(OBJ)/%.o, $(SOURCES))
else
	SOURCES := $(wildcard $(SRC)/*.c)
	OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))
endif

LDFLAGS = -lm


$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) -I$(SRC) -c $< -o $@ $(CFLAGS)

all: conway 

conway: $(OBJECTS) 
	$(CC) $(OBJECTS) -o conway -lm

clean:
	$(RM) -f $(OBJECTS) conway *.pbm

$(OBJ):
	mkdir -p $(OBJ)