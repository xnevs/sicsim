#ifndef NODE_H
#define NODE_H

#include "opcode.h"

#include <stddef.h>

enum node_type {
	NT_NULL,
	NT_COMMENT,
	NT_DIRECTIVE,
	NT_STORAGE,
	NT_INSTF1,
	NT_INSTF2,
	NT_INSTF3,
};

struct Node {
	enum node_type type;

	char label[256];

	enum opcode mnemonic;

	int extended;

	char operands[265];
	char operands_vanilla[256];

	int nixbpe;
	long op;

	char comment[256];

	char *data;

	long loc;
	int size;

	struct Node *next;
};

int Node_init(struct Node *node, char *tokens[], int token_count, int comment, long loc);

int Node_emitText(struct Node *node, unsigned char *buf, int size);

int Node_printNode(struct Node *node);

#endif
