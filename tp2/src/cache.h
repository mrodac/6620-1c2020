#ifndef CACHE_H_
#define CACHE_H_

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define MEM_SIZE 65536
#define CACHE_SIZE 4096
#define LINE_SIZE 128
#define WAYS 4

struct Line {
   bool valid;
   unsigned int tag;
   unsigned int last;
   unsigned char data[LINE_SIZE];
};

struct Set {
   unsigned int last;
   struct Line ways[WAYS];
};

void init();

void flush();

unsigned int get_offset (unsigned int address);

unsigned int find_set(unsigned int address);

unsigned int select_oldest(unsigned int setnum);

int compare_tag (unsigned int tag, unsigned int set);

void read_tocache(unsigned int blocknum, unsigned int way, unsigned int set);

void write_tocache(unsigned int address, unsigned char value);

unsigned char read_byte(unsigned int address);

void write_byte(unsigned int address, unsigned char value);

float get_miss_rate();

#endif /* CACHE_H_ */
