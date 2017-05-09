#ifndef CODE_H
#define CODE_H

#include "Machine.h"

#include <stdio.h>

struct Node;

struct Symbol {
	char label[256];
	long value;

	struct Symbol *next;
};

struct Symtab {
	struct Symbol *first;
	struct Symbol *last;
};

int Symtab_init(struct Symtab *symtab);
int Symtab_insert(struct Symtab *symtab, char *label, long loc);
long Symtab_get(struct Symtab *symtab, char *label);
int Symtab_print(struct Symtab *symtab, FILE *f);

struct Code {
	char name[7];
	long start;
	long end;
	long code_len;

	struct Node *first;
	struct Node *last;

	long loc;

	struct Symtab *symtab;
};

int Code_init(struct Code *code);
int Code_delete(struct Code *code);

int Code_addNode(struct Code *code, struct Node *node);
int Code_parseLine(struct Code *code, char *line);
int Code_parse(struct Code *code, FILE *asm_file);
int Code_generate_obj(struct Code *code, FILE *obj_file);
int Code_generate_log(struct Code *code, FILE *log_file);
int Code_generate_lst(struct Code *code, FILE *lst_file);
int Code_generate_mem(struct Code *code, unsigned char *mem, long n, struct Machine *machine);

#endif
