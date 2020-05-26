#include <regdef.h>

.text
.abicalls
.align 2

/*
unsigned int vecinos(unsigned char *matriz, unsigned int i, unsigned int j, unsigned int filas, unsigned int columnas) {
    unsigned int v = 0;
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            if (y || x) {
                int I = y+i;
                if (I < 0) I += filas;
                if (I == filas) I = 0;

                int J = x+j;
                if (J < 0) J += columnas;
                if (J == columnas) J = 0;

                v += matriz[I * columnas + J];
            }
        }
    }
    return v;
}

Stack:
	8|	fp
sp	0|	$gp
*/

.globl	vecinos
.ent vecinos

vecinos:
	subu sp, sp, 8
	sw gp, 4(sp)
	sw fp, 0(sp)
	move fp, sp

	move v0, zero

	//TODO

	lw gp, 4(sp)
	lw fp, 0(sp)
	addu sp, sp, 8
	jr ra

.end vecinos
