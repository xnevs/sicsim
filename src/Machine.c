#include "Machine.h"

#include "opcode.h"
#include "common.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

int Machine_init(struct Machine *machine) {
	machine->memory = malloc(MAX_ADDRESS+1);

	memset(machine->devices, 0, sizeof(machine->devices));
	machine->devices[0] = stdin;
	machine->devices[1] = stdout;
	machine->devices[2] = stderr;

	machine->SW = 0;
	machine->PC = 0;

	machine->A &= 0xFFFFFF;
	machine->X &= 0xFFFFFF;
	machine->L &= 0xFFFFFF;
	machine->B &= 0xFFFFFF;
	machine->S &= 0xFFFFFF;
	machine->T &= 0xFFFFFF;

	machine->freq = 100;

	return 0;
}
int Machine_delete(struct Machine *machine) {
	free(machine->memory);
	int i;
	for(i=0; i<=MAX_DEVICE; i++) {
		if(machine->devices[i] != NULL && machine->devices[i] != stdin &&
		   machine->devices[i] != stdout && machine->devices[i] != stderr) {
			fclose(machine->devices[i]);
			machine->devices[i] = NULL;
		}
	}

	return 0;
}

#define getReg(machine, regno, data) Machine_getReg(machine, regno, data)
int Machine_getReg(struct Machine *machine, enum reg regno, long *data) {
	switch(regno) {
	case REG_A:
		*data = machine->A;
		break;
	case REG_X:
		*data = machine->X;
		break;
	case REG_L:
		*data = machine->L;
		break;
	case REG_B:
		*data = machine->B;
		break;
	case REG_S:
		*data = machine->S;
		break;
	case REG_T:
		*data = machine->T;
		break;
	default:
		return -1;
	}
	return 0;
}

#define setReg(machine, regno, val) Machine_setReg(machine, regno, val)
int Machine_setReg(struct Machine *machine, enum reg regno, long val) {
	val &= 0xFFFFFF;
	switch(regno) {
	case REG_A:
		machine->A = val;
		break;
	case REG_X:
		machine->X = val;
		break;
	case REG_L:
		machine->L = val;
		break;
	case REG_B:
		machine->B = val;
		break;
	case REG_S:
		machine->S = val;
		break;
	case REG_T:
		machine->T = val;
		break;
	case REG_PC:
		machine->PC = val;
		break;
	case REG_SW:
		machine->SW = val;
		break;
	default:
		return -1;
	}
	return 0;
}

int Machine_clearReg(struct Machine *machine) {
	machine->PC = 0;
	machine->SW = 0;
	machine->A = 0;
	machine->X = 0;
	machine->L = 0;
	machine->B = 0;
	machine->S = 0;
	machine->T = 0;
	machine->F = 0;

	return 0;
}

#define getByte(machine, addr, data) Machine_getByte(machine, addr, data)
int Machine_getByte(struct Machine *machine, long addr, unsigned char *data) {
	if(addr < 0 || addr > MAX_ADDRESS)
		return -1;
	*data = machine->memory[addr];
	return 0;
}

#define setByte(machine, addr, val) Machine_setByte(machine, addr, val)
int Machine_setByte(struct Machine *machine, long addr, unsigned char val) {
	if(addr < 0 || addr > MAX_ADDRESS)
		return -1;
	machine->memory[addr] = val;
	return 0;
}

#define getWord(machine, addr, data) Machine_getWord(machine, addr, data)
int Machine_getWord(struct Machine *machine, long addr, long *data) {
	if(addr < 0 || addr+2 > MAX_ADDRESS)
		return -1;
	*data = machine->memory[addr] << 16;
	*data |= machine->memory[addr+1] << 8;
	*data |= machine->memory[addr+2];
	return 0;
}

#define setWord(machine, addr, val) Machine_setWord(machine, addr, val)
int Machine_setWord(struct Machine *machine, long addr, long val) {
	if(addr < 0 || addr+2 > MAX_ADDRESS)
		return -1;
	machine->memory[addr]   = (unsigned char) (( val >> 16) & 0xFF);
	machine->memory[addr+1] = (unsigned char) (( val >> 8) & 0xFF);
	machine->memory[addr+2] = (unsigned char) (val & 0xFF);
	return 0;
}

#define getFloat(machine, addr, data) Machine_getFloat(machine, addr, data)
int Machine_getFloat(struct Machine *machine, long addr, double *data) {
	if(addr < 0 || addr+5 > MAX_ADDRESS)
		return -1;
	unsigned long long data_bits;
	data_bits = (unsigned long long)machine->memory[addr] << 40;
	data_bits |= (unsigned long long)machine->memory[addr+1] << 32;
	data_bits |= (unsigned long long)machine->memory[addr+2] << 24;
	data_bits |= (unsigned long long)machine->memory[addr+3] << 16;
	data_bits |= (unsigned long long)machine->memory[addr+4] << 8;
	data_bits |= (unsigned long long)machine->memory[addr+5];

	data_bits <<= 16;

	*data = *(double *)&data_bits;
	return 0;
}

#define setFloat(machine, addr, val) Machine_setFloat(machine, addr, val)
int Machine_setFloat(struct Machine *machine, long addr, double val) {
	if(addr < 0 || addr+5 > MAX_ADDRESS)
		return -1;
	unsigned long long data_bits = *(unsigned long long *)&val;
	data_bits >>= 16;
	machine->memory[addr] = (unsigned char) ((data_bits >> 40) & 0xFF);
	machine->memory[addr+1] = (unsigned char) ((data_bits >> 32) & 0xFF);
	machine->memory[addr+2] = (unsigned char) ((data_bits >> 24) & 0xFF);
	machine->memory[addr+3] = (unsigned char) ((data_bits >> 16) & 0xFF);
	machine->memory[addr+4] = (unsigned char) ((data_bits >> 8) & 0xFF);
	machine->memory[addr+5] = (unsigned char) (data_bits & 0xFF);
	return 0;
}

int Machine_clearMem(struct Machine *machine) {
	memset(machine->memory, 0, MAX_ADDRESS);
	return 0;
}


#define getDevice(machine, num, device) Machine_getDevice(machine, num, device)
int Machine_getDevice(struct Machine *machine, int num, FILE **device) {
	if(num < 0 || num > 255)
		return -1;
	*device = machine->devices[num];
	return 0;
}

#define setDevice(machine, num, device) Machine_setDevice(machine, num, device)
int Machine_setDevice(struct Machine *machine, int num, FILE *device) {
	if(num < 0 || num > 255)
		return -1;
	if(machine->devices[num] != NULL)
		fclose(machine->devices[num]);

	machine->devices[num] = device;
	return 0;
}

int Machine_loadObj(struct Machine *machine, FILE *obj_file) {
	char name[7];
	long code_len;

	char line[72]; /* kako dolga je lahko vrstica? */
	while(fgets(line, sizeof(line), obj_file)) {
		int len = strlen(line);
		if(line[len-1] == '\n')
			line[--len] = '\0';

		char type = line[0];
		if(type == 'H') {
			strncpy(name, line+1, 6);
			name[6] = '\0';
			char str[7];
			strncpy(str, line+13, 6);
			code_len = strtol(str, NULL, 16);
		}
		else if(type == 'T') {
			char str[7];
			strncpy(str, line+1, 6);
			str[6] = '\0';
			long addr = strtol(str, NULL, 16);

			strncpy(str, line+7, 2);
			str[2] = '\0';
			int count = strtol(str, NULL, 16);

			int i;
			unsigned char value;
			for(i = 0; i < count; i++) {
				strncpy(str, line+9+2*i, 2);
				str[2] = '\0';
				value = (unsigned char)strtol(str, NULL, 16);
				setByte(machine, addr+i, value);
			}
		}
		else if(type == 'E') {
			char str[7];
			strncpy(str, line+1, 6);
			str[6] = '\0';
			long start_addr = strtol(str, NULL, 16);
			machine->PC = start_addr & 0xFFFFFF;
		}
	}

	int i;
	for(i=3; i<=MAX_DEVICE; i++) {
		if(machine->devices[i]) {
			fclose(machine->devices[i]);
			machine->devices[i] = NULL;
		}
	}

	return 0;
}

long comp(long a, long b) {
	a = s24(a);
	b = s24(b);
	if(a < b)
		return 0x000040;
	else if(a == b)
		return 0x000000;
	else
		return 0x000080;
}
long compf(double a, double b) {
	if(a < b)
		return 0x000040;
	else if(a == b)
		return 0x000000;
	else
		return 0x000080;
}
/* EXECUTE */
int execF1(struct Machine *machine, enum opcode opcode) {
	switch(opcode) {
		case OPCODE_FLOAT:
			machine->F = (double)machine->A;
			break;
		case OPCODE_FIX:
			setReg(machine, REG_A, (long)machine->F);
			break;
		case OPCODE_NORM:
		case OPCODE_SIO:
		case OPCODE_HIO:
		case OPCODE_TIO:
			break;
		default:
			return 1;
	}
	return 0;
}
int execF2(struct Machine *machine, enum opcode opcode, long operand) {
	int v1 = (operand & 0xF0) >> 4;
	int v2 = operand & 0x0F;
	switch(opcode) {
		case OPCODE_CLEAR: {
			setReg(machine, v1, 0);
			break;
		}
		case OPCODE_RMO: {
			long R1;
			getReg(machine, v1, &R1);
			setReg(machine, v2, R1);
			break;
		}
		case OPCODE_SHIFTL: {
			long R1;
			getReg(machine, v1, &R1);
			R1 = (R1 << (v2+1)) | (R1 >> (24 - (v2 + 1)));
			setReg(machine, v1, R1);
			break;
		}
		case OPCODE_SHIFTR: {
			long R1;
			getReg(machine, v1, &R1);
			R1 = s24(R1);
			R1 >>= v2 + 1;
			setReg(machine, v1, R1);
			break;
		}
		case OPCODE_COMPR: {
			long R1, R2;
			getReg(machine, v1, &R1);
			getReg(machine, v2, &R2);
			machine->SW = comp(R1, R2);
			break;
		}
		case OPCODE_TIXR: {
			setReg(machine, REG_X, machine->X + 1);
			long R1;
			getReg(machine, v1, &R1);
			machine->SW = comp(machine->X, R1);
			break;
		}
		case OPCODE_ADDR: {
			long R1, R2;
			getReg(machine, v1, &R1);
			getReg(machine, v2, &R2);
			setReg(machine, v2, R2+R1);
			break;
		}
		case OPCODE_SUBR: {
			long R1, R2;
			getReg(machine, v1, &R1);
			getReg(machine, v2, &R2);
			setReg(machine, v2, R2-R1);
			break;
		}
		case OPCODE_MULR: {
			long R1, R2;
			getReg(machine, v1, &R1);
			getReg(machine, v2, &R2);
			setReg(machine, v2, R2*R1);
			break;
		}
		case OPCODE_DIVR: {
			long R1, R2;
			getReg(machine, v1, &R1);
			getReg(machine, v2, &R2);
			setReg(machine, v2, R2/R1);
			break;
		}
		case OPCODE_SVC: {
			break;
		}
		default:
			return 1;
	}
	return 0;
}

int loadByte(struct Machine *machine, int ni, long operand, unsigned char *ret_op) {
	if(ni == 1)
		*ret_op = (unsigned char)operand;
	else if(ni == 2) {
		long addr;
		if(getWord(machine, operand, &addr) < 0)
			return -1;
		if(getByte(machine, addr, ret_op) < 0)
			return -1;
	}
	else
		if(getByte(machine, operand, ret_op) < 0)
			return -1;
	return 0;
}
int loadWord(struct Machine *machine, int ni, long operand, long *ret_op) {
	if(ni == 1) {
		*ret_op = s12(operand);
		return 0;
	}
	if(getWord(machine, operand, ret_op) < 0)
		return -1;
	if(ni == 2) {
		operand = *ret_op;
		if(getWord(machine, operand, ret_op) < 0)
			return -1;
	}
	return 0;
}
int loadFloat(struct Machine *machine, int ni, long operand, double *ret_op) {
	if(ni == 1) {
		*ret_op = (double)s12(operand);
	}
	else if(ni == 2) {
		long addr;
		if(getWord(machine, operand, &addr) < 0)
			return -1;
		if(getFloat(machine, addr, ret_op) < 0)
			return -1;
	}
	else {
		if(getFloat(machine, operand, ret_op) < 0)
			return -1;
	}
	return 0;
}
int storeByte(struct Machine *machine, int ni, long operand, unsigned char value) {
	if(ni == 1)
		return -1;
	if(ni == 2) {
		long addr;
		if(getWord(machine, operand, &addr) < 0)
			return -1;
		operand = addr;
	}
	if(setByte(machine, operand, value) < 0)
		return -1;
	return 0;
}
int storeWord(struct Machine *machine, int ni, long operand, long value) {
	if(ni == 1)
		return -1;
	if(ni == 2) {
		long addr;
		if(getWord(machine, operand, &addr) < 0)
			return -1;
		operand = addr;
	}
	if(setWord(machine, operand, value) < 0)
		return -1;
	return 0;
}
int storeFloat(struct Machine *machine, int ni, long operand, double value) {
	if(ni == 1)
		return -1;
	if(ni == 2) {
		long addr;
		if(getWord(machine, operand, &addr) < 0)
			return -1;
		operand = addr;
	}
	if(setFloat(machine, operand, value) < 0)
		return -1;
	return 0;
}
int testDevice(struct Machine *machine, unsigned char devno) {
	if(machine->devices[devno] == NULL) {
		char str[10];
		sprintf(str, "%02X.dev", devno);
		machine->devices[devno] = fopen(str, "a+");
	}
	if(feof(machine->devices[devno]))
		return 0x000000;
	else
		return 0x000080;
}
int readDevice(struct Machine *machine, unsigned char devno) {
	if(machine->devices[devno] == NULL) {
		char str[10];
		sprintf(str, "%02X.dev", devno);
		machine->devices[devno] = fopen(str, "a+");
	}
	char c = fgetc(machine->devices[devno]);
	if(c != EOF) {
		machine->A = c;
		return 0;
	}
	else {
		return -1;
	}
}
int writeDevice(struct Machine *machine, unsigned char devno) {
	if(machine->devices[devno] == NULL) {
		char str[10];
		sprintf(str, "%02X.dev", devno);
		machine->devices[devno] = fopen(str, "a+");
	}
	fputc(machine->A & 0xFF, machine->devices[devno]);
	fflush(machine->devices[devno]);
	return 0;
}
int execSICF3F4(struct Machine *machine, enum opcode opcode, int ni, long operand) {
	long value;
	switch(opcode) {
		case OPCODE_LDA: {
			loadWord(machine, ni, operand, &value);
			setReg(machine, REG_A, value);
			break;
		}
		case OPCODE_LDCH: {
			unsigned char byte;
			loadByte(machine, ni, operand, &byte);
			setReg(machine, REG_A, byte);
			break;
		}
		case OPCODE_LDX: {
			loadWord(machine, ni, operand, &value);
			setReg(machine, REG_X, value);
			break;
		}
		case OPCODE_LDL: {
			loadWord(machine, ni, operand, &value);
			setReg(machine, REG_L, value);
			break;
		}
		case OPCODE_LDB: {
			loadWord(machine, ni, operand, &value);
			setReg(machine, REG_B, value);
			break;
		}
		case OPCODE_LDS: {
			loadWord(machine, ni, operand, &value);
			setReg(machine, REG_S, value);
			break;
		}
		case OPCODE_LDF: {
			loadFloat(machine, ni, operand, &machine->F);
			break;
		}
		case OPCODE_LDT: {
			loadWord(machine, ni, operand, &value);
			setReg(machine, REG_T, value);
			break;
		}
		case OPCODE_LPS: {
			break;
		}
		case OPCODE_STA: {
			storeWord(machine, ni, operand, machine->A);
			break;
		}
		case OPCODE_STCH: {
			storeByte(machine, ni, operand, (unsigned char)(machine->A & 0xFF));
			break;
		}
		case OPCODE_STX: {
			storeWord(machine, ni, operand, machine->X);
			break;
		}
		case OPCODE_STL: {
			storeWord(machine, ni, operand, machine->L);
			break;
		}
		case OPCODE_STB: {
			storeWord(machine, ni, operand, machine->B);
			break;
		}
		case OPCODE_STS: {
			storeWord(machine, ni, operand, machine->S);
			break;
		}
		case OPCODE_STF: {
			storeFloat(machine, ni, operand, machine->F);
			break;
		}
		case OPCODE_STT: {
			storeWord(machine, ni, operand, machine->T);
			break;
		}
		case OPCODE_STI: {
			break;
		}
		case OPCODE_STSW: {
			storeWord(machine, ni, operand, machine->SW);
			break;
		}
		case OPCODE_AND: {
			loadWord(machine, ni, operand, &value);
			setReg(machine, REG_A, machine->A & value);
			break;
		}
		case OPCODE_OR: {
			loadWord(machine, ni, operand, &value);
			setReg(machine, REG_A, machine->A | value);
			break;
		}
		case OPCODE_COMP: {
			loadWord(machine, ni, operand, &value);
			machine->SW = comp(machine->A, value);
			break;
		}
		case OPCODE_COMPF: {
			double double_value;
			loadFloat(machine, ni, operand, &double_value);
			machine->SW = compf(machine->F, double_value);
			break;
		}
		case OPCODE_TIX: {
			setReg(machine, REG_X, machine->X+1);
			loadWord(machine, ni, operand, &value);
			machine->SW = comp(machine->X, value);
			break;
		}
		case OPCODE_JEQ: {
			if(machine->SW == 0x000000) {
				if(ni == 3)
					loadWord(machine, 1, operand, &value);
				else
					loadWord(machine, ni, operand, &value);
				machine->PC = value;
			}
			break;
		}
		case OPCODE_JGT: {
			if(machine->SW == 0x000080) {
				if(ni == 3)
					loadWord(machine, 1, operand, &value);
				else
					loadWord(machine, ni, operand, &value);
				machine->PC = value;
			}
			break;
		}
		case OPCODE_JLT: {
			if(machine->SW == 0x000040) {
				if(ni == 3)
					loadWord(machine, 1, operand, &value);
				else
					loadWord(machine, ni, operand, &value);
				machine->PC = value;
			}
			break;
		}
		case OPCODE_J: {
			if(ni == 3)
				loadWord(machine, 1, operand, &value);
			else
				loadWord(machine, ni, operand, &value);
			machine->PC = value;
			break;
		}
		case OPCODE_JSUB: {
			machine->L = machine->PC;
			if(ni == 3)
				loadWord(machine, 1, operand, &value);
			else
				loadWord(machine, ni, operand, &value);
			machine->PC = value;
			break;
		}
		case OPCODE_RSUB: {
			machine->PC = machine->L;
			break;
		}
		case OPCODE_ADD: {
			loadWord(machine, ni, operand, &value);
			setReg(machine, REG_A, machine->A + value);
			break;
		}
		case OPCODE_SUB: {
			loadWord(machine, ni, operand, &value);
			setReg(machine, REG_A, machine->A - value);
			break;
		}
		case OPCODE_MUL: {
			loadWord(machine, ni, operand, &value);
			setReg(machine, REG_A, machine->A * value);
			break;
		}
		case OPCODE_DIV: {
			loadWord(machine, ni, operand, &value);
			setReg(machine, REG_A, machine->A / value);
			break;
		}
		case OPCODE_ADDF: {
			double float_value;
			loadFloat(machine, ni, operand, &float_value);
			machine->F += float_value;
			break;
		}
		case OPCODE_SUBF: {
			double float_value;
			loadFloat(machine, ni, operand, &float_value);
			machine->F -= float_value;
			break;
		}
		case OPCODE_MULF: {
			double float_value;
			loadFloat(machine, ni, operand, &float_value);
			machine->F *= float_value;
			break;
		}
		case OPCODE_DIVF: {
			double float_value;
			loadFloat(machine, ni, operand, &float_value);
			machine->F /= float_value;
			break;
		}
		case OPCODE_RD: {
			unsigned char devno;
			loadByte(machine, ni, operand, &devno);
			readDevice(machine, devno);
			break;
		}
		case OPCODE_WD: {
			unsigned char devno;
			loadByte(machine, ni, operand, &devno);
			writeDevice(machine, devno);
			break;
		}
		case OPCODE_TD: {
			//machine->SW = 0x000080/* ne vem kaj */;
			unsigned char devno;
			loadByte(machine, ni, operand, &devno);
			machine->SW = testDevice(machine, devno);
			break;
		}
		case OPCODE_SSK: {
			break;
		}
		default:
			return 1;
	}
	return 0;
}
int fetch(struct Machine *machine, unsigned char *data) {
	if(getByte(machine, machine->PC, data) < 0)
		return -1;
	machine->PC = (machine->PC + 1) & 0xFFFFFF;
	return 0;

}
int Machine_execute(struct Machine *machine) {
	unsigned char opcode;
	if(fetch(machine, &opcode) < 0)
		return -1;

	int retcode;
	/* Format 1 */
	retcode = execF1(machine, opcode);
	if(retcode  < 0) return -1;
	if(retcode == 0) return 0;
	
	long operand;
	unsigned char data;
	if(fetch(machine, &data) < 0)
		return -1;
	operand = data;
	/* Format 2 */
	retcode = execF2(machine, opcode, operand);
	if(retcode  < 0) return -1;
	if(retcode == 0) return 0;

	int ni = opcode & 3;
	opcode &= 0xFC;
	
	if(fetch(machine, &data) < 0)
		return -1;
	operand = (operand << 8) | data;

	if(ni == 0) {
		int flag = operand & 0x8000;
		operand &= 0x7FFF;
		if(flag) /* indexed? */
			operand += machine->X;
	}
	else {
		int flags = (operand & 0xF000) >> 12;
		operand &= 0x0FFF;
		if(flags & 1) { /* extended? */
			fetch(machine, &data);
			operand = (operand << 8) | data;
		}
		else if(flags & 2 && !(flags & 4)) /* PC relative? */
			operand = s12(operand) + machine->PC;
		else if(flags & 4) /* base relative? */
			operand += machine->B;
		if(flags & 8) /* indexed? */
			operand += machine->X;
	}

	operand &= 0xFFFFFF;
	retcode = execSICF3F4(machine, opcode, ni, operand);
	if(retcode  < 0) return -1;

	return 0;
}

int Machine_run(struct Machine *machine) {
	int count = machine->freq;

	int clocks_for_one_instr = CLOCKS_PER_SEC / count;
	
	clock_t sstart = clock();
	long prevPC = -1;
	while(count-- && machine->PC != prevPC) {
		clock_t start = clock();
		prevPC = machine->PC;
		if(Machine_execute(machine) < 0)
			return -1;

		int sleep_time = clocks_for_one_instr - (clock() - start);
		usleep(sleep_time > 0 ? sleep_time : 0);
	}

	double ddiff = (((double)clock() - sstart) / (double)CLOCKS_PER_SEC);
	
	int sleep_time = 1000000 - (int)(ddiff * 1000000);
	usleep(sleep_time > 0 ? sleep_time : 0);
	
	if(machine->PC == prevPC)
		return 0;
	else
		return 1;
}

int Machine_print(struct Machine *machine) {
	printf("################################\n");
	printf("  PC = 0x%lX\n", machine->PC);
	printf("  SW = 0x%lX\n", machine->SW);
	printf("   A = 0x%lX\t%ld\n", machine->A, s24(machine->A));
	printf("   X = 0x%lX\t%ld\n", machine->X, s24(machine->X));
	printf("   L = 0x%lX\t%ld\n", machine->L, s24(machine->L));
	printf("   B = 0x%lX\t%ld\n", machine->B, s24(machine->B));
	printf("   S = 0x%lX\t%ld\n", machine->S, s24(machine->S));
	printf("   T = 0x%lX\t%ld\n", machine->T, s24(machine->T));
	printf("   F = %lf\n", machine->F);
	printf("################################\n");

	return 0;
}
