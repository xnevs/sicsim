#include "assembler.h"

#include "Code.h"

#include <stdio.h>

int assembler(FILE *asm_file, FILE *obj_file, FILE *log_file, FILE *lst_file) {
	struct Code code;
	Code_init(&code);

	Code_parse(&code, asm_file);

	Code_generate_obj(&code, obj_file);
	Code_generate_log(&code, log_file);
	Code_generate_lst(&code, lst_file);

	return 0;
}
