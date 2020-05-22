#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

#define ERROR -1
#define SUCCESS 0

/*
extern unsigned int vecinos(unsigned char *a, unsigned int i, unsigned int j,
        unsigned int M, unsigned int N);
*/

void mostrar_ayuda() {
    printf("%s", "Uso:\n\t"
            "conway -h\n\t"
            "conway -V\n\t"
            "conway i M N inputfile [-o outputprefix]\n"
            "Opciones:\n\t"
            "-h, --help\t\tImprime este mensaje.\n\t"
            "-V, --version \tDa la versión del programa.\n\t"
            "-o Prefijo de los archivos de salida.\n\t"
            "Ejemplos:\n\t"
            "conway 10 20 20 glider -o estado\n "
            "Representa 10 iteraciones del Juego de la Vida en una matriz de 20x20,\n "
            "con un estado inicial tomado del archivo ‘‘glider’’.\n "
            "Los archivos de salida se llamarán estado_n.pbm.\n "
            "Si no se da un prefijo para los archivos de salida,\n "
            "el prefijo será el nombre del archivo de entrada.\n");
}

int openFile(FILE** file, const char *path, const char *mode) {
    *file = fopen(path, mode);
    if (*file == NULL) {
        fprintf(stderr, "Error abriendo el archivo '%s': ", path);
        perror(NULL);
        return ERROR;
    }
    return SUCCESS;
}

int closeFile(FILE* file) {
    if (file == NULL)
        return ERROR;

    if (fclose(file) == ERROR) {
        perror("Error cerrando archivo");
        return ERROR;
    }
    return SUCCESS;
}

bool at_eof(FILE* file) {
    return feof(file) != 0;
}

bool has_error(FILE* file) {
    return ferror(file) != 0;
}

int processFile(char* matriz, char* inputpath, int M, int N, char* outputprefix) {
    int status = SUCCESS;

    FILE* inputfile;

    if (status == SUCCESS && inputpath && strcmp(inputpath, "-") != 0)
        status = openFile(&inputfile, inputpath, "r");
    else
        inputfile = stdin;

    if (status != ERROR) {
        // TODO
    }

    closeFile(inputfile);

    return status;
}

int parsearEntero(char* cstr, int* entero) {
    char* endptr = cstr;
    *entero = strtol(cstr, &endptr, 10);

    if (endptr && *endptr != '\0') {
        fprintf(stderr, "Error en argumentos: '%s' no es un entero\n", cstr);
        return ERROR;
    }
    return SUCCESS;
}

int main(int argc, char *argv[]) {
    int status = SUCCESS;

    int M=0, N=0;
    char *inputpath = 0;
    char *outputprefix = 0;

    static struct option longOpts[] = {
            { "help", no_argument, NULL, 'h' },
            { "version", no_argument, NULL, 'V' },
            { "o", required_argument, NULL, 'o' },
            { NULL, 0, NULL, 0 }
    };

    int c;
    unsigned short index = 0;
    do {
        c = getopt_long(argc, argv, "-hVo:", longOpts, NULL);

        switch (c) {
        case 1:
            if (index == 0)
                status = parsearEntero(optarg, &M);
            else if (index == 1)
                status = parsearEntero(optarg, &N);
            else if (index == 2)
                inputpath = optarg;
            else
                status = ERROR;

            if (status == ERROR) {
                mostrar_ayuda();
                return ERROR;
            }
            index+=1;
            break;
        case 'V':
            printf("%s", "conway v0.1\n");
            return SUCCESS;
        case 'h':
            mostrar_ayuda();
            return SUCCESS;
        case 'o':
            outputprefix = optarg;
            break;
        case '?':
            mostrar_ayuda();
            return ERROR;
        default:
            break;
        }
    } while (c != -1);

    unsigned char* matriz = 0;

    if (M > 1 && N > 1) {
        matriz = malloc(M * N);
        if (! matriz) {
            fprintf(stderr, "Error reservando memoria\n");
            status = ERROR;
        }
    }

    if (status != ERROR)
        status = processFile(matriz, inputpath, M, N, outputprefix);

    if (matriz)
        free(matriz);

    return status;
}
