#include "cache.h"

//Memoria principal
unsigned char mem[MEM_SIZE];

//Conjuntos del cache. Para los valores dados son 8.
struct Set sets[CACHE_SIZE / (LINE_SIZE * WAYS)];

//Cantidad de accesos a memoria y misses
unsigned int accessed;
unsigned int missed;

//Mask para obtener el numero de set y el offset desde la dirección
unsigned int offsetMask;
unsigned int setMask;

//Cantidad de sets que tiene el cache
unsigned int numOfSets;	//8

//Cantidad de bits para el offset y para obtener el numero de set.
unsigned int offsetBits; //7
unsigned int setBits;	//3

//logaritmo base dos para obtener cantidad de bits
unsigned int log2int(unsigned int n) {
    int r = 0;
    while (n >>= 1) ++r;
    return r;
}

void init() {
	//Inicialidar la memoria en 0
    for (int i = 0; i < MEM_SIZE; i++) {
        mem[i] = 0;
    }

    numOfSets = CACHE_SIZE / (LINE_SIZE * WAYS);
    offsetBits = log2int(LINE_SIZE);
    setBits = log2int(numOfSets);

    offsetMask = LINE_SIZE - 1; //0000000001111111
    setMask = numOfSets * LINE_SIZE - 1; //0000001111111111

    flush();
}

//Resetea el bit valid en las vias y el campo last usado para la pilitica de remplazo
void flush() {
    for (int i = 0; i < numOfSets; i++) {
        sets[i].last = 0;
        for (int j = 0; j < WAYS; j++) {
            sets[i].ways[j].valid = false;
            sets[i].ways[j].last = 0;
        }
    }
}

unsigned int get_offset(unsigned int address) {
    return address & offsetMask;
}

unsigned int find_set(unsigned int address) {
    return (address & setMask) >> offsetBits;
}

unsigned int get_tag(unsigned int address) {
    return address >> (offsetBits + setBits);
}

//Devuelve el numero de vía más antiguo o el primero no valido
unsigned int select_oldest(unsigned int setnum) {
    struct Set* set = &sets[setnum];

    unsigned int oldest = set->last + 1;
    int waynum = -1;
    for (int i = 0; i < WAYS; i++) {
        struct Line* line = &set->ways[i];
        if ((waynum < 0 && ! line->valid) || line->last < oldest) {
            waynum = i;
            oldest = line->last;
        }
    }
    return waynum;
}

//Devuelve el numero de vía con mismo tag del parámetro para un conjunto
int compare_tag(unsigned int tag, unsigned int setnum) {
    struct Set* set = &sets[setnum];
    for (int i = 0; i < WAYS; i++) {
        if (set->ways[i].valid && set->ways[i].tag == tag)
            return i;
    }
    return -1;
}

//Copia un bloque de memoria a una linea de cache, seteando el tag y bit de validez
void read_tocache(unsigned int blocknum, unsigned int wayNum, unsigned int setnum) {
    struct Set* set = &sets[setnum];
    struct Line* line = &set->ways[wayNum];

    memcpy(&line->data, &mem[blocknum * LINE_SIZE],LINE_SIZE);

    line->tag = get_tag(blocknum * LINE_SIZE);
    line->valid = true;
}

//Escribe un byte al bloque de cache correspondiente según address
void write_tocache(unsigned int address, unsigned char value) {
    unsigned int setnum = find_set(address);
    unsigned int tag = get_tag(address);
    unsigned int offset = get_offset(address);
    int wayNum = compare_tag(tag, setnum);

    struct Set* set = &sets[setnum];
    struct Line* line = &set->ways[wayNum];

    line->last = ++set->last;
    line->data[offset] = value;
}

//Lee un byte desde el cache, copiando un bloque desde memoria si hace falta
unsigned char read_byte(unsigned int address) {
    accessed++;

    unsigned int setnum = find_set(address);
    unsigned int tag = get_tag(address);
    int wayNum = compare_tag(tag, setnum);

    struct Set* set = &sets[setnum];

    if (wayNum < 0) {
        missed++;
        wayNum = select_oldest(setnum);
        read_tocache(address / LINE_SIZE, wayNum, setnum);
    }

    set->ways[wayNum].last = ++set->last;

    int offset = get_offset(address);
    return set->ways[wayNum].data[offset];
}

//Escribe un byte a memoria y al cache si el bloque está presente
void write_byte(unsigned int address, unsigned char value) {
    accessed++;

    unsigned int setnum = find_set(address);
    unsigned int tag = get_tag(address);
    int wayNum = compare_tag(tag, setnum);

    if (wayNum < 0) {
        missed++;
    } else {
        write_tocache(address, value);
    }

    mem[address] = value;
}

//Devuelve el miss rate como porcentaje
float get_miss_rate() {
    return accessed > 0 ? (missed / (float) accessed) : 0.0;
}
