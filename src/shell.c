#include "shell.h"

#include "Machine.h"
#include "disasm.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_TOKENS 10

int exit_loop;
char *tokens[MAX_TOKENS];

typedef int (*command_t)(struct Machine *machine);
int run_command(struct Machine *machine) {
	while(Machine_run(machine) > 0);
	return 0;
}
int step_command(struct Machine *machine) {
	if(tokens[1] == NULL)
		Machine_execute(machine);
	else {
		int count = strtol(tokens[1], NULL, 0);
		int i;
		for(i=0; i<count; i++)
			Machine_execute(machine);
	}
	return 0;
}
int print_command(struct Machine *machine) {
	Machine_print(machine);
	return 0;
}
int sp_command(struct Machine *machine) {
	Machine_execute(machine);
	Machine_print(machine);
	return 0;
}
int getreg_command(struct Machine *machine) {
	if(strcmp(tokens[1], "A") == 0)
		printf("   A = 0x%lX\t(%ld)\n", machine->A, machine->A);
	else if(strcmp(tokens[1], "X") == 0)
		printf("   X = 0x%lX\t(%ld)\n", machine->X, machine->X);
	else if(strcmp(tokens[1], "L") == 0)
		printf("   L = 0x%lX\t(%ld)\n", machine->L, machine->L);
	else if(strcmp(tokens[1], "B") == 0)
		printf("   B = 0x%lX\t(%ld)\n", machine->B, machine->B);
	else if(strcmp(tokens[1], "S") == 0)
		printf("   S = 0x%lX\t(%ld)\n", machine->S, machine->S);
	else if(strcmp(tokens[1], "T") == 0)
		printf("   T = 0x%lX\t(%ld)\n", machine->T, machine->T);
	else if(strcmp(tokens[1], "F") == 0)
		printf("   F = %lf\n", machine->F);
	else if(strcmp(tokens[1], "PC") == 0)
		printf("  PC = 0x%lX\n", machine->PC);
	else if(strcmp(tokens[1], "SW") == 0)
		printf("  SW = 0x%lX\n", machine->SW);
	return 0;
}
int setreg_command(struct Machine *machine) {
	if(strcmp(tokens[1], "A") == 0) {
		long value = strtol(tokens[2], NULL, 0);
		Machine_setReg(machine, REG_A, value);
	}
	else if(strcmp(tokens[1], "X") == 0) {
		long value = strtol(tokens[2], NULL, 0);
		Machine_setReg(machine, REG_X, value);
	}
	else if(strcmp(tokens[1], "L") == 0) {
		long value = strtol(tokens[2], NULL, 0);
		Machine_setReg(machine, REG_L, value);
	}
	else if(strcmp(tokens[1], "B") == 0) {
		long value = strtol(tokens[2], NULL, 0);
		Machine_setReg(machine, REG_B, value);
	}
	else if(strcmp(tokens[1], "S") == 0) {
		long value = strtol(tokens[2], NULL, 0);
		Machine_setReg(machine, REG_S, value);
	}
	else if(strcmp(tokens[1], "T") == 0) {
		long value = strtol(tokens[2], NULL, 0);
		Machine_setReg(machine, REG_T, value);
	}
	else if(strcmp(tokens[1], "F") == 0) {
		double value = strtod(tokens[2], NULL);
		machine->F = value;
	}
	else if(strcmp(tokens[1], "PC") == 0) {
		long value = strtol(tokens[2], NULL, 0);
		machine->PC = value & 0xFFFFFF;
	}
	else if(strcmp(tokens[1], "SW") == 0) {
		long value = strtol(tokens[2], NULL, 0);
		machine->SW = value & 0xFFFFFF;
	}
	return 0;
}
int getbyte_command(struct Machine *machine) {
	long addr = strtol(tokens[1], NULL, 0);
	unsigned char data;
	Machine_getByte(machine, addr, &data);
	printf("mem[0x%06lX] = 0x%02X\n", addr, data);
	return 0;
}
int setbyte_command(struct Machine *machine) {
	long addr = strtol(tokens[1], NULL, 0);
	unsigned char value = (unsigned char)(strtol(tokens[2], NULL, 0) & 0xFF);
	Machine_setByte(machine, addr, value);
	return 0;
}
int getword_command(struct Machine *machine) {
	long addr = strtol(tokens[1], NULL, 0);
	long data;
	Machine_getWord(machine, addr, &data);
	printf("mem[0x%06lX..] = 0x%06lX\n", addr, data);
	return 0;
}
int setword_command(struct Machine *machine) {
	long addr = strtol(tokens[1], NULL, 0);
	long value = (long)(strtol(tokens[2], NULL, 0) & 0xFFFFFF);
	Machine_setWord(machine, addr, value);
	return 0;
}
int printmem_command(struct Machine *machine) {
	printf("            0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
	long addr = (strtol(tokens[1], NULL, 0) / 0x10) * 0x10;
	long count = strtol(tokens[2], NULL, 0);
	int i;
	for(i=0; i<count; i++) {
		printf("0x%06lX:  ", addr);
		int j;
		for(j=0; j<0x10; j++) {
			if(j > 0)
				printf(" ");
			unsigned char data;
			Machine_getByte(machine, addr+j, &data);
			printf("%02X", data);
		}
		printf("\n");
		addr += 0x10;
	}
	return 0;
}
int setmem_command(struct Machine *machine) {
	long addr = strtol(tokens[1], NULL, 0);
	int i;
	for(i=2; tokens[i] != NULL; i++) {
		unsigned char value = strtol(tokens[i], NULL, 16);
		Machine_setByte(machine, addr, value);
		addr++;
	}
	return 0;
}
int setdev_command(struct Machine *machine) {
	long devno = strtol(tokens[1], NULL, 16);
	Machine_setDevice(machine, devno, fopen(tokens[2], "rw"));
	return 0;
}
int load_command(struct Machine *machine) {
	FILE *obj_file = fopen(tokens[1], "r");
	Machine_loadObj(machine, obj_file);
	return 0;
}
int disasm_command(struct Machine *machine) {
	long addr = strtol(tokens[1], NULL, 0);
	int num = strtol(tokens[2], NULL, 0);

	disasm(&machine->memory[addr], addr, num);

	return 0;
}
int exit_command(struct Machine *machine) {
	exit_loop = 1;
	return 0;
}

char *help_str[][2] = {
	{"run", "run machine"},
	{"step", "step"},
	{"print", "print registers"},
	{"sp", "step and print"},
	{"getreg", "get regiter"},
	{"setreg", "set register"},
	{"getbyte", "get byte"},
	{"setbyte", "set byte"},
	{"getword", "get word"},
	{"setword", "set word"},
	{"printmem", "print memory"},
	{"setmem", "set memory"},
	{"load", "load obj file"},
	{"disasm", "disassemble"},
	{"exit", "exit"}};

int help_command(struct Machine *machine) {
	int i;
	for(i=0; i<15; i++) {
		printf("%s\t%s\n", help_str[i][0], help_str[i][1]);
	}
	return 0;
}


struct {
	char *command_name;
	command_t command;
} commands[] = {
	{"run", run_command},
	{"step", step_command},
	{"print", print_command},
	{"sp", sp_command},
	{"getreg", getreg_command},
	{"setreg", setreg_command},
	{"getbyte", getbyte_command},
	{"setbyte", setbyte_command},
	{"getword", getword_command},
	{"setword", setword_command},
	{"printmem", printmem_command},
	{"setmem", setmem_command},
	{"setdev", setdev_command},
	{"load", load_command},
	{"disasm", disasm_command},
	{"help", help_command},
	{"exit", exit_command}};

command_t get_command(char *command_name) {
	size_t i;
	for(i=0; i<sizeof(commands)/sizeof(commands[0]); i++) {
		if(strcmp(commands[i].command_name, command_name) == 0)
			return commands[i].command;
	}
	return NULL;
}


int tokenize(char *line) {
	int token_count = 0;
	int len = strlen(line);
	int i;
	for(i=0; i<len; i++) {
		if(isspace(line[i]))
			line[i] = '\0';
		else if(i == 0 || line[i-1] == '\0')
			tokens[token_count++] = &line[i];

	}
	tokens[token_count] = (char *)NULL;
	return token_count;
}

int shell(struct Machine *machine) {
	char line[256];
	
	exit_loop = 0;
	while(!exit_loop) {
		printf("> ");
		fgets(line, sizeof(line), stdin);
		if(line[strlen(line)-1] == '\n')
			line[strlen(line)-1] = '\0';
		if(tokenize(line) > 0) {
			command_t command = get_command(tokens[0]);
			if(command)
				command(machine);
			else
				printf("Error: Unrecognized command.\n");
		}
	}

	return 0;
}
