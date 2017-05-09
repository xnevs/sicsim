#include "Code.h"

#include "Node.h"
#include "Machine.h"

#include "Node.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int Symbol_init(struct Symbol *symbol, char *label, long loc) {
	strncpy(symbol->label, label, sizeof(symbol->label));
	symbol->value = loc;

	symbol->next = NULL;
	return 0;
}

int Symtab_init(struct Symtab *symtab) {
	symtab->first = NULL;
	symtab->last = NULL;
	return 0;
}

int Symtab_delete(struct Symtab *symtab) {
	struct Symbol *symbol = symtab->first, *symbol1 = NULL;
	while(symbol != NULL) {
		symbol1 = symbol->next;
		free(symbol);
		symbol = symbol1;
	}
	symtab->first = NULL;
	symtab->last = NULL;

	return 0;
}

int Symtab_insert(struct Symtab *symtab, char *label, long loc) {
	struct Symbol *symbol = (struct Symbol *)malloc(sizeof(struct Symbol));
	Symbol_init(symbol, label, loc);

	if(symtab->last == NULL) {
		symtab->first = symbol;
		symtab->last = symbol;
	}
	else {
		symtab->last->next = symbol;
		symtab->last = symtab->last->next;
	}

	return 0;
}

long Symtab_get(struct Symtab *symtab, char *label) {
	struct Symbol *symbol = symtab->first;

	while(symbol != NULL && strcmp(symbol->label, label) != 0) {
		symbol = symbol->next;
	}
	if(symbol != NULL)
		return symbol->value;
	else
		return -1;
}

int Symtab_print(struct Symtab *symtab, FILE *f) {
	struct Symbol *symbol = symtab->first;
	while(symbol != NULL) {
		fprintf(f, "%s\t0x%06X\n", symbol->label, symbol->value);
		symbol = symbol->next;
	}
}

int Code_init(struct Code *code) {
	code->name[0] = '\0';
	code->start = 0;
	code->end = 0;
	code->code_len = 0;

	code->first = NULL;
	code->last = NULL;

	code->loc = 0;

	code->symtab = (struct Symtab *)malloc(sizeof(struct Symtab));
	Symtab_init(code->symtab);

	return 0;
}

int Code_delete(struct Code *code) {
	struct Node *node = code->first, *node1 = NULL;
	while(node != NULL) {
		node1 = node->next;
		free(node);
		node = node1;
	}
	code->first == NULL;
	code->last == NULL;

	Symtab_delete(code->symtab);
	free(code->symtab);
	code->symtab = NULL;

	return 0;
}

int Code_addNode(struct Code *code, struct Node *node) {
	if(code->last == NULL) {
		code->first = node;
		code->last = node;
	}
	else {
		code->last->next = node;
		code->last = code->last->next;
	}

	return 0;
}

int Code_parseLine(struct Code *code, char *line) {
	int len = strlen(line);
	int cnt = 0;
	int comment = -1;
	char *tokens[4] = {"", "", "", ""};
	int i;
	for(i=0; i<len && cnt<4; i++) {
		if(line[i] == '.') {
			comment = cnt;
			tokens[cnt++] = &line[i+1];
			break;
		}
		else if( isblank(line[i]) ) {
			if(cnt == 0)
				cnt++;
			line[i] = '\0';
		}
		else if( i==0 || ((line[i-1]=='\0') && !isblank(line[i])) ) {
			tokens[cnt++] = &line[i];
		}
	}

	if(cnt > 0) {
		struct Node *node = malloc(sizeof(struct Node));

		Node_init(node, tokens, cnt, comment, code->loc);

		if(node->mnemonic == DIRECTIVE_START) {
			code->loc = node->op;
		}
		else if(node->mnemonic == DIRECTIVE_EQU) {
			Symtab_insert(code->symtab, node->label, node->op);
		}
		else if(node->label[0]) { //mogoce else
			Symtab_insert(code->symtab, node->label, node->loc); //spremenil iz code->loc
		}

		Code_addNode(code, node);

		if(node->mnemonic != DIRECTIVE_START && node->mnemonic != DIRECTIVE_ORG)
			return code->loc + node->size;
		else
			return node->op & 0xFFFFFF;
	}

	return code->loc;
}

int Code_parse(struct Code *code, FILE *asm_file) {
	code->loc = 0;

	char line[1024];
	while(fgets(line, sizeof(line), asm_file) != NULL) {
		if(line[strlen(line)-1] == '\n')
			line[strlen(line)-1] = '\0';
		code->loc = Code_parseLine(code, line);
	}

	code->start = -1;
	code->end = -1;
	code->code_len = 0;
	code->loc = 0;
	struct Node *node = code->first;
	while(node != NULL) {
		Node_operandResolution(node, code);

		if(node->mnemonic != DIRECTIVE_START && node->mnemonic != DIRECTIVE_ORG)
			code->loc += node->size;
		else {
			code->loc = node->op;

			if(node->mnemonic == DIRECTIVE_START) {
				if(code->start != -1)
					return -1;
				code->start = node->op;
				strncpy(code->name, node->label, 6);
				code->name[6] = '\0';

			}
		}

		if(node->mnemonic == DIRECTIVE_END) {
			if(code->end != -1)
				return -1;
			code->end = node->op;
		}

		code->code_len += node->size;
		node = node->next;
	}

	if(code->start == -1) code->start = 0;
	if(code->end == -1) code->end = 0;

	return 0;
}


int print_buf(char str[], unsigned char *buf, int size) {
	int i;
	for(i=0; i<size; i++) {
		sprintf(str+(2*i), "%02X", buf[i]);
	}
	return 0;
}
int write_line(long addr, char *line, int line_len, FILE *file) {
	fprintf(file, "T%06X%02X", addr, line_len / 2);
	fputs(line, file);
	fprintf(file, "\n");
}
int Code_generate_obj(struct Code *code, FILE *obj_file) {
	//H record
	fprintf(obj_file, "H%-6s%06X%06X\n", code->name, code->start, code->code_len);

	//T records
	char line[60];
	line[0] = '\0';
	long line_loc = -1;
	char inst[256];
	long loc = code->first->loc;
	char str[256];

	struct Node *node = code->first;
	while(node != NULL) {
		if(node->type == NT_INSTF1 ||
		   node->type == NT_INSTF2 ||
		   node->type == NT_INSTF3 ||
		   node->mnemonic == STORAGE_WORD ||
		   node->mnemonic == STORAGE_BYTE) {
		   	
			int size = Node_emitText(node, inst, sizeof(inst));

			if(2*size > 60)
				return -1;

			if( (node->loc != loc) || (2*size > 60-strlen(line)) ) {
				write_line(line_loc, line, strlen(line), obj_file);
				line[0] = '\0';
				loc = node->loc;
			}

			if(strlen(line) == 0)
				line_loc = node->loc;

			print_buf(str, inst, size);
			strncat(line, str, sizeof(line));
		}

		//tukaj je problem, da RESW in RESB samo premakneta loc
		if(node->mnemonic != STORAGE_RESW && node->mnemonic != STORAGE_RESB)
			loc += node->size;

		node = node->next;
	}
	if(strlen(line) > 0)
		write_line(line_loc, line, strlen(line), obj_file);

	//E record
	fprintf(obj_file, "E%06X\n", code->end);

	return 0;
}

int Code_generate_log(struct Code *code, FILE *log_file) {
	fprintf(log_file, "Symbols\n");
	Symtab_print(code->symtab, log_file);

	return 0;
}


int Code_generate_lst(struct Code *code, FILE *lst_file) {
	struct Node *node = code->first;

	unsigned char buf[256];
	char str[256];
	while(node != NULL) {
		if(node->type == NT_INSTF1 ||
		   node->type == NT_INSTF2 ||
		   node->type == NT_INSTF3 ||
		   node->mnemonic == STORAGE_WORD ||
		   node->mnemonic == STORAGE_BYTE) {

			fprintf(lst_file, "%05X\t", node->loc);

			int size = Node_emitText(node, buf, sizeof(buf));
			print_buf(str, buf, size);
			fprintf(lst_file, "%s\t", str);

			fprintf(lst_file, "%s\t", node->label);

			opcode_to_str(node->mnemonic, str, sizeof(str));
			fprintf(lst_file, "%s\t", str);

			fprintf(lst_file, "%s", node->operands_vanilla);
			fprintf(lst_file, "\n");
		}

		node = node->next;
	}


	return 0;
}

int Code_generate_mem(struct Code *code, unsigned char *mem, long n, struct Machine *machine) {
	struct Node *node = code->first;

	char buf[256];
	machine->PC = code->end;
	while(node != NULL) {
		if(node->loc + node->size > n)
			return -1;

		int size = Node_emitText(node, buf, sizeof(buf));
		memcpy(mem+node->loc, buf, size);

		node = node->next;
	}

	return 0;
}
