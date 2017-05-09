#include "shell.h"
#include "Machine.h"

int main() {
	struct Machine machine;
	Machine_init(&machine);

	shell(&machine);

	Machine_delete(&machine);

	return 0;
}
