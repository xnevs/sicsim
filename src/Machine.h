#ifndef MACHINE_H
#define MACHINE_H

#include <stdio.h>

#define MAX_ADDRESS 0xFFFFF
#define MAX_DEVICE 255

struct Machine {
	long A;
	long X;
	long L;
	long B;
	long S;
	long T;
	double F;

	long PC;
	long SW;

	unsigned char *memory;

	FILE *devices[MAX_DEVICE+1];

	int freq;
};

enum reg {REG_A=0, REG_X=1, REG_L=2, REG_B=3, REG_S=4, REG_T=5, REG_F=6, REG_SW, REG_PC};

int Machine_init(struct Machine *machine);
int Machine_delete(struct Machine *machine);

int Machine_getReg(struct Machine *machine, enum reg regno, long *data);
int Machine_setReg(struct Machine *machine, enum reg regno, long val);
int Machine_clearReg(struct Machine *machine);
int Machine_getByte(struct Machine *machine, long addr, unsigned char *data);
int Machine_setByte(struct Machine *machine, long addr, unsigned char val);
int Machine_getWord(struct Machine *machine, long addr, long *data);
int Machine_setWord(struct Machine *machine, long addr, long val);
int Machine_clearMem(struct Machine *machine);
int Machine_getDevice(struct Machine *machine, int num, FILE **device);
int Machine_setDevice(struct Machine *machine, int num, FILE *device);

int Machine_loadObj(struct Machine *machine, FILE *obj_file);

int Machine_execute(struct Machine *machine);
int Machine_run(struct Machine *machine);

int Machine_print(struct Machine *machine);

#endif
