#include <stdio.h>
#include <string.h>

#include "assembler.h"

int main(int argc, char *argv[]) {
	FILE *asm_file = fopen(argv[1], "r");
	
	char *bname = strrchr(argv[1], '/');
	if(bname != NULL)
		bname++;
	else
		bname = argv[1];

	char fname[1024];
	strncpy(fname, bname, sizeof(fname));
	*strrchr(fname, '.') = '\0';

	char obj_name[1024];
	strncpy(obj_name, fname, sizeof(obj_name));
	strncat(obj_name, ".obj", sizeof(obj_name));
	FILE *obj_file = fopen(obj_name, "w");

	char log_name[1024];
	strncpy(log_name, fname, sizeof(log_name));
	strncat(log_name, ".log", sizeof(log_name));
	FILE *log_file = fopen(log_name, "w");

	char lst_name[1024];
	strncpy(lst_name, fname, sizeof(lst_name));
	strncat(lst_name, ".lst", sizeof(lst_name));
	FILE *lst_file = fopen(lst_name, "w");

	assembler(asm_file, obj_file, log_file, lst_file);

	return 0;
}
