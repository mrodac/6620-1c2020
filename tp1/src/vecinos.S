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
sp 0|	$gp
*/

//labels para los parámetros
#define matriz a0
#define i a1
#define j a2
#define filas a3
#define columnas t0

//labes para variables temporales en registros
#define x t1
#define y t2
#define I t3
#define J t4
#define cmp t5 
#define idx t6
#define val t7
#define addr t8
#define one t9

.globl	vecinos
.ent vecinos

vecinos:
	subu sp, sp, 8		//Stack de 8 bytes.
	sw gp, 4(sp)
	sw fp, 0(sp)
	move fp, sp

	lw columnas, 24(sp) //Obtener el quinto parámetro del stack de la funcion llamante

	move v0, zero		// v = 0
	li one, 1			// cte
	li y, -1			//y = -1

loopY:	
	li x, -1			//x = -1

loopX:
	or cmp, y, x		//(y || x)
	beqz cmp, continue
	nop

	add I, y, i			// I = y+i;
	bltz I, IltZero		// if (I < 0)
	nop
	beq I, filas, IeqFilas	//(I == filas)
	nop

Icorrected:
	add J, x, j			//J = x+j;
	bltz J, JltZero		//if (J < 0)
	nop
	bge J, columnas, JeqColumnas	//if (J >= columnas)
	nop

Jcorrected:
	mult I, columnas	// I * filas
	mflo idx
	add idx, idx, J		// + J

	add addr, matriz, idx	//matriz[I * columnas + J]

	lbu val, 0(addr)

	addu v0, v0, val	// v+= matriz[I * columnas + J]

continue:
	add x, x, one 		// x++

	ble x, one, loopX	// x <= 1
	nop

	add y, y, one 		// y++

	ble y, one, loopY	// y <= 1
	nop

	lw gp, 4(sp)		// Restaurar gp y fp
	lw fp, 0(sp)		// desdde la SRA
	addu sp, sp, 8		// Destruir el stack
	jr ra				//return v

IltZero:
	add I, I, filas		//I += filas
	b Icorrected
	nop

IeqFilas:
	move I, zero		//I = 0
	b Icorrected
	nop

JltZero:
	add J, J, columnas	//J += columnas
	b Jcorrected
	nop

JeqColumnas:
	move J, zero	//J = 0
	b Jcorrected
	nop

.end vecinos

