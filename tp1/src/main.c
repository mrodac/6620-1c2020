#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#define ERROR -1
#define SUCCESS 0

#define LINEA 100


/*
extern unsigned int vecinos(unsigned char *a, unsigned int i, unsigned int j,
        unsigned int M, unsigned int N);
*/

unsigned int vecinos(unsigned char *matriz, unsigned int i, unsigned int j, unsigned int M, unsigned int N) {

    unsigned int v = 0;
    for (int y = -1; y < 2; y++) {
        for (int x = -1; x < 2; x++) {
            if (y || x) {
                unsigned int I = (y+i)% M, J = (x+j)%N;
                if (matriz[I * M + J])
                    v++;
            }
        }
    }
    return v;
}


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

int parsearEntero(char** str, int* entero, char tail) {
    char* endptr = *str;
    *entero = strtol(*str, &endptr, 10);

    if (endptr == *str || *endptr != tail) {
        return ERROR;
    }

    *str=endptr+1;
    return SUCCESS;
}

int cargarMatriz(unsigned char* matriz, char* inputpath, unsigned int M, unsigned int N) {
    int status = SUCCESS;

    FILE* inputfile;

    if (status == SUCCESS && inputpath && strcmp(inputpath, "-") != 0)
        status = openFile(&inputfile, inputpath, "r");
    else
        inputfile = stdin;

    if (status != ERROR) {
        for (int idx = 0; idx < M * N; idx++)
            matriz[idx] = 0;

        char linea[LINEA];
        int i, j;

        while (status != ERROR && fgets (linea, LINEA, inputfile) != NULL) {
            char* ptr = linea;
            status = parsearEntero(&ptr, &i, ' ');

            if (status == ERROR)
                break;

            status = parsearEntero(&ptr, &j, '\n');
            if (status == ERROR)
                break;

            if (i < 0 || i >= M || j < 0 || j >= N)
                status = ERROR;
            else
                matriz[i * M + j] = 1;
        }

        if (status == ERROR)
            fprintf(stderr, "Formato de coordenadas no válido: '%s': ", linea);

        if (ferror(inputfile)) {
            fprintf(stderr, "Error leyendo el archivo '%s': ", inputpath);
            perror(NULL);
            status = ERROR;
        }

    }

    closeFile(inputfile);

    return status;
}

void mostrarMatriz(unsigned char* matriz, unsigned int M, unsigned int N, FILE* output) {

    fputc('/', output);
    for (int j = 0; j < N; j++) {
        fprintf(output, "--");
    }

    printf("\\\n");
    for (int i = 0; i < M; i++) {
        fputc('|', output);

        for (int j = 0; j < N; j++) {
            if (matriz[i * M + j])
                fprintf(output, "()");
            else
                fprintf(output,"  ");
        }
        fprintf(output, "|\n");
    }

    fputc('\\', output);
    for (int j = 0; j < N; j++) {
        fprintf(output, "--");
    }
    fprintf(output, "/\n");

    usleep(50 * 1000);
}

/**
Si una celda tiene menos de dos o más de tres vecinos encendidos, su siguiente estado es apagado.
Si una celda encendida tiene dos o tres vecinos encendidos, su siguiente estado es encendido.
Si una celda apagada tiene exactamente tres vecinos encendidos, su siguiente estado es encendido.
* */


int procesar(unsigned char** matriz, char* outputprefix, unsigned int steps, unsigned int M, unsigned int N) {
    for (int step = 0 ; step < steps; step++) {
        unsigned char* siguiente = malloc(M * N);

        if (! siguiente) {
            fprintf(stderr, "Error reservando memoria\n");
            return ERROR;
        }

        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                unsigned char estado = (*matriz)[i * M + j];
                unsigned int v = vecinos(*matriz, i, j, M, N);

                if (v < 2 || v > 3)
                    estado = 0;
                else if (estado && (v == 2 || v == 3))
                    estado = 1;
                else if (! estado && v == 3)
                    estado = 1;

                siguiente[i * M + j] = estado;
            }
        }

        free(*matriz);
        *matriz = siguiente;
        mostrarMatriz(*matriz, M, N, stdout);
    }

    return SUCCESS;
}




int main(int argc, char *argv[]) {
    int status = SUCCESS;

    unsigned int steps=0, M=0, N=0;
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
            if (index < 3) {
                int i;
                status = parsearEntero(&optarg, &i, '\0');
                if (status != ERROR && i < 0)
                    status = ERROR;

                if (index == 0)
                    steps = i;
                else if (index == 1)
                    M = i;
                else if (index == 2)
                    N = i;

            } else if (index == 3)
                inputpath = optarg;
            else
                status = ERROR;

            if (status == ERROR) {
                if (index < 3)
                    fprintf(stderr, "Error en argumentos: '%s' no es un entero\n", optarg);
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

    if (index < 4) {
        mostrar_ayuda();
        return ERROR;
    }

    unsigned char* matriz = 0;

    if (M > 1 && N > 1) {
        matriz = malloc(M * N);
        if (! matriz) {
            fprintf(stderr, "Error reservando memoria\n");
            status = ERROR;
        }
    }

    if (status != ERROR)
        status = cargarMatriz(matriz, inputpath, M, N);

    if (status != ERROR) {
        mostrarMatriz(matriz, M, N, stdout);
        status = procesar(&matriz, outputprefix, steps, M, N);
    }

    if (matriz)
        free(matriz);

    return status;
}
