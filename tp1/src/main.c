#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <unistd.h>

#include "vecinos.h"

#define ERROR -1
#define OK 0

void mostrar_ayuda() {
    printf("%s", "Uso:\n\t"
            "conway -h\n\t"
            "conway -V\n\t"
            "conway i M N inputfile [-o outputprefix] [-d] [-n] \n"
            "Opciones:\n\t"
            "-h, --help\tImprime este mensaje.\n\t"
            "-V, --version \tDa la versión del programa.\n\t"
            "-d, --display\tMuestra el estado de cada iteración por la salida estandar.\n\t"
            "-o\t\tPrefijo de los archivos de salida.\n\t"
            "-n, --novideo\tOmite la generación de un video en base a los archivos de salida.\n"
            "Ejemplos:\n"
            " conway 10 20 20 glider -o estado\n"
            " Representa 10 iteraciones del Juego de la Vida en una matriz de 20x20,\n "
            " con un estado inicial tomado del archivo ‘‘glider’’.\n "
            " Los archivos de salida se llamarán estado_n.pbm.\n "
            " Si no se da un prefijo para los archivos de salida,\n "
            " el prefijo será el nombre del archivo de entrada.\n");
}


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

int cargarMatriz(unsigned char* matriz, char* entrada, unsigned int filas, unsigned int columnas) {
    int status = OK;
    FILE* archivo;
    status = abrirArchivo(&archivo, entrada, "r");

    if (status != ERROR) {
        for (int idx = 0; idx < filas * columnas; idx++)
            matriz[idx] = 0;

        int i, j;

        do {
            if (fscanf(archivo, "%d %d\n", &i, &j) != 2) {
                if (! feof(archivo) || !ferror(archivo)) {
                    fprintf(stderr, "Error de formato en el archivo de entrada\n");
                    status = ERROR;
                }
            } else {
                if (i < 0 || i >= filas || j < 0 || j >= columnas) {
                    fprintf(stderr, "Coordenadas fuera de la matriz: (%d;%d)\n", i, j);
                    status = ERROR;
                } else
                    matriz[i * columnas + j] = 1;
            }
        } while (status != ERROR && ! feof(archivo));

        if (ferror(archivo)) {
            fprintf(stderr, "Error leyendo el archivo '%s': ", entrada);
            perror(NULL);
            status = ERROR;
        }
    }

    cerrarArchivo(archivo);

    return status;
}

int guardarImagen(unsigned char* matriz, unsigned int filas, unsigned int columnas, char* salida, unsigned char escala) {
    int status = OK;
    FILE* archivo;
    status = abrirArchivo(&archivo, salida, "w");

    if (status != ERROR) {
        if (fprintf(archivo, "P4\n%d %d\n", columnas * escala, filas * escala) > 0) {
        	for (int i = 0; i < filas && status != ERROR; i++) {
            	for (int h = 0; h < escala && status != ERROR; h++) {
					for (int j = 0; j < columnas && status != ERROR; j++) {
						char c = matriz[i * columnas + j] ? 0 : 255;

						for (int w = 0; w < escala && status != ERROR; w+=8)
						    status = fputc(c, archivo) < 0 ? ERROR : OK;
					}
            	}
            }
        }

        if (ferror(archivo)) {
            fprintf(stderr, "Error escribiendo en el archivo '%s': ", salida);
            perror(NULL);
            status = ERROR;
        }
    }

    cerrarArchivo(archivo);
    return status;
}

void mostrarEstado(unsigned char* matriz, unsigned int filas, unsigned int columnas) {
    printf("/");
    for (int j = 0; j < columnas; j++) {
        printf("--");
    }

    printf("\\\n");
    for (int i = 0; i < filas; i++) {
        printf("|");

        for (int j = 0; j < columnas; j++) {
            if (matriz[i * columnas + j])
                printf("()");
            else
                printf("  ");
        }
        printf("|\n");
    }

    printf("\\");
    for (int j = 0; j < columnas; j++) {
        printf("--");
    }
    printf("/\n");
}

/**
Si una celda tiene menos de dos o más de tres vecinos encendidos, su siguiente estado es apagado.
Si una celda encendida tiene dos o tres vecinos encendidos, su siguiente estado es encendido.
Si una celda apagada tiene exactamente tres vecinos encendidos, su siguiente estado es encendido.
* */
int siguiente(unsigned char** matriz, unsigned int filas, unsigned int columnas) {
    int status = OK;
	unsigned char* siguiente = malloc(filas * columnas);

	if (! siguiente) {
		fprintf(stderr, "Error reservando memoria\n");
		status = ERROR;
	}

	if (status == OK) {
		for (int i = 0; i < filas; i++) {
			for (int j = 0; j < columnas; j++) {
				unsigned char estado = (*matriz)[i * columnas + j];
				unsigned int v = vecinos(*matriz, i, j, filas, columnas);

				if (v < 2 || v > 3)
					estado = 0;
				else if (estado && (v == 2 || v == 3))
					estado = 1;
				else if (! estado && v == 3)
					estado = 1;

				siguiente[i * columnas + j] = estado;
			}
		}
	}

	if (siguiente) {
		free(*matriz);
		*matriz = siguiente;
	}

    return status;
}


int parsearParametros(int argc, char *argv[], unsigned int* pasos, unsigned int* filas, unsigned int* columnas, char** entrada, char** prefijoSalida, bool* mostrarEstados, bool* video) {
    int status = OK;

    static struct option longOpts[] = {
            { "help", no_argument, NULL, 'h' },
            { "version", no_argument, NULL, 'V' },
            { "o", required_argument, NULL, 'o' },
            { "display", no_argument, NULL, 'd' },
            { "novideo", no_argument, NULL, 'n' },
            { NULL, 0, NULL, 0 }
    };

    int c;
    unsigned short index = 0;
    do {
        c = getopt_long(argc, argv, "-hVo:dn", longOpts, NULL);

        switch (c) {
        case 1:
            if (index < 3) {
                int entero = 0;
                if (sscanf(optarg, "%d", &entero) != 1 || entero < 0)
                    status = ERROR;

                if (index == 0)
                    *pasos = entero;
                else if (index == 1)
                    *filas = entero;
                else if (index == 2)
                    *columnas = entero;

            } else if (index == 3)
                *entrada = optarg;
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
            printf("%s", "conway v0.2\n");
            return OK;
        case 'h':
            mostrar_ayuda();
            return OK;
        case 'o':
            *prefijoSalida = optarg;
            break;
        case 'd':
            *mostrarEstados = true;
            break;
        case 'n':
            *video = false;
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

    if (! *prefijoSalida)
        *prefijoSalida = *entrada;

    return status;
}


int generarVideo(char* prefijoSalida, unsigned char digitos) {
    unsigned int tamPrefijoSalida = strlen(prefijoSalida);

    char* salida = malloc(tamPrefijoSalida + 5); //extensión
    char* archivos = malloc(tamPrefijoSalida + 10); //"_%0Xd.pbm"

    if (! archivos || ! salida) {
        fprintf(stderr, "Error reservando memoria\n");
        return ERROR;
    }

    snprintf(salida, tamPrefijoSalida + 5, "%s.mp4", prefijoSalida);

    snprintf(archivos, tamPrefijoSalida + 10, "%s_%%0%dd.pbm", prefijoSalida, digitos);

    printf("Generando %s...\n", salida);

    int rv = execl("/usr/bin/ffmpeg", "ffmpeg", "-y", "-framerate", "10", "-i", archivos, "-loglevel", "panic", "-r", "10", salida, (char*) NULL);

    if (salida)
        free(salida);

    if (archivos)
        free(archivos);

    return rv;
}


int main(int argc, char *argv[]) {
    unsigned int pasos=0, filas=0, columnas=0;
    char *entrada = 0;
    char *prefijoSalida = 0;
    bool mostrarEstados = false;
    bool video = true;

    int status = parsearParametros(argc, argv, &pasos, &filas, &columnas, &entrada, &prefijoSalida, &mostrarEstados, &video);
    unsigned char digitos = 3;

    if (pasos > 0) {
        digitos = floor(log10(pasos))+1;
        if (digitos > 9) {
            fprintf(stderr, "Usar una cantidad de pasos menor a 10^9\n");
            status = ERROR;
        }
        if (digitos < 3)
            digitos = 3;
    }

    if (status != ERROR) {
        unsigned int salidalen = strlen(prefijoSalida) + digitos + 6; //6: len('_.pbm\0')
        char* salida = malloc(salidalen);

        char formatoSalida[] = "%s_%0?d.pbm";
        formatoSalida[5] = digitos + 48; //número a texto

        unsigned char* matriz = malloc(filas * columnas);

        if (! matriz || ! salida) {
            fprintf(stderr, "Error reservando memoria\n");
            status = ERROR;
        }

        if (status != ERROR)
            status = cargarMatriz(matriz, entrada, filas, columnas);

        if (status != ERROR) {
            if (mostrarEstados)
                mostrarEstado(matriz, filas, columnas);

            unsigned int escalaX = 1920 / columnas / 8;
            unsigned int escalaY = 1080 / filas / 8;
            unsigned char escala = escalaX < escalaY ? escalaX : escalaY;
            escala = escala > 0 ? escala * 8 : 8;

            snprintf(salida, salidalen, formatoSalida, prefijoSalida, 0);
			status = guardarImagen(matriz, filas, columnas, salida, escala);

            for (int paso = 1 ; paso <= pasos && status != ERROR; paso++) {
                status = siguiente(&matriz, filas, columnas);
                if (mostrarEstados)
                    mostrarEstado(matriz, filas, columnas);

                snprintf(salida, salidalen, formatoSalida, prefijoSalida, paso);
                status = guardarImagen(matriz, filas, columnas, salida, escala);
            }
        }

        if (matriz)
            free(matriz);

        if (salida)
            free(salida);

        if (status != ERROR && pasos > 0 && video) {
            if (generarVideo(prefijoSalida, digitos) < 0) {
                fprintf(stderr, "Error invocando a ffmpeg\n");
                status = ERROR;
            }
        }

    }

    return status;
}
