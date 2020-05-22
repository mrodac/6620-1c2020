# Opciones para el compilador C/C++ para tratamiento de errores y warnings.
CFLAGS = -std=c99 -Wall -Werror -pedantic -pedantic-errors

# Para valgrind o debug
#CFLAGS += -ggdb -fno-inline -g3

.PHONY: all
SRC := src
OBJ := obj

SOURCES := $(wildcard $(SRC)/*.c $(SRC)/*.s)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) -I$(SRC) -c $< -o $@ $(CFLAGS)

all: conway 

conway: $(OBJECTS) 
	$(CC) $(OBJECTS) -o conway

clean:
	$(RM) -f $(OBJECTS) conway

$(OBJ):
	mkdir -p $(OBJ)