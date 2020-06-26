#include "cache.h"
#include <stdio.h>

#define ERROR -1
#define OK 0

int abrirArchivo(FILE** file, const char *nombre, const char *modo) {
    *file = fopen(nombre, modo);
    if (*file == NULL) {
        fprintf(stderr, "Error abriendo el archivo '%s': ", nombre);
        perror(NULL);
        return ERROR;
    }
    return OK;
}

int cerrarArchivo(FILE* file) {
    if (file == NULL)
        return ERROR;

    if (fclose(file) == ERROR) {
        perror("Error cerrando archivo");
        return ERROR;
    }
    return OK;
}

int procesarArchivo(FILE* archivo) {
    int status = OK;

    do {
        char command[7];
        unsigned int address;
        unsigned int value;

        int read = fscanf(archivo, "%6s %u, %u\n", command, &address, &value);

        if (read > 0) {
            if (strncmp(command, "FLUSH", 6) == 0) {
                flush();
            } else if (strncmp(command, "MR", 3) == 0) {
                printf("Miss rate: %%%f\n", 100 * get_miss_rate());
            } else if (strncmp(command, "R", 2) == 0 && read == 2) {
                if (address >= MEM_SIZE) {
                    fprintf(stderr, "Direccion de memoria %u fuera de rango\n", address);
                    status = ERROR;
                    break;
                }
                value = read_byte(address);
                printf("Byte at %u is %hhu\n", address, value);
            } else if (strncmp(command, "W", 2) == 0 && read == 3) {
                if (address >= MEM_SIZE) {
                    fprintf(stderr, "Direccion de memoria %u fuera de rango\n", address);
                    status = ERROR;
                    break;
                }

                if (value > 255) {
                    fprintf(stderr, "%u No es un byte válido\n", value);
                    status = ERROR;
                    break;
                }

                write_byte(address, value);
                printf("Byte at %u set to %hhu\n", address, value);
            } else {
                fprintf(stderr, "Comando invalido: %s", command);
                if (read > 1)
                    fprintf(stderr, " %u", address);
                if (read > 2)
                    fprintf(stderr, ", %hhu", value);
                fprintf(stderr, "\n");

                status = ERROR;
            }
        }
    } while (status != ERROR && ! feof(archivo));

    return status;
}

int main(int argc, char *argv[]) {
    int status = OK;
    FILE* archivo;

    if (argc == 2) {
        status = abrirArchivo(&archivo, argv[1], "r");
        if (status == OK) {
            init();
            procesarArchivo(archivo);
            cerrarArchivo(archivo);
        }
    } else {
        status = ERROR;
        fprintf(stderr, "Error en argumentos: Se debe proporcionar la ruta de UN archivo para procesar\n");
    }

    return status;
}
