CC='gcc'
CFLAGS='-g'

GTKFLAG=`pkg-config --cflags --libs gtk+-2.0`

all: sim_shell sim_gui asm

sim_gui: gui_main.o Machine.o Code.o Node.o shell.o disasm.o common.o opcode.o
	$(CC) $(CFLAGS) $(GTKFLAG) -o gsim gui_main.o Machine.o Code.o Node.o shell.o disasm.o common.o opcode.o

sim_shell: shell_main.o Machine.o shell.o disasm.o common.o
	$(CC) $(CFLAGS) -o sim shell_main.o Machine.o shell.o disasm.o common.o

gui_main.o: src/gui_main.c src/Machine.h src/Code.h
	$(CC) $(CFLAGS) -c src/gui_main.c $(GTKFLAG)

shell_main.o: src/shell_main.c src/Machine.h
	$(CC) $(CFLAGS) -c src/shell_main.c

Machine.o: src/Machine.c src/Machine.h src/common.h
	$(CC) $(CFLAGS) -c src/Machine.c

common.o: src/common.c src/common.h
	$(CC) $(CFLAGS) -c src/common.c

opcode.o: src/opcode.c src/opcode.h
	$(CC) $(CFLAGS) -c src/opcode.c

shell.o: src/shell.c src/shell.h src/Machine.h
	$(CC) $(CFLAGS) -c src/shell.c

disasm.o: src/disasm.c src/disasm.h src/Machine.h src/opcode.h src/common.h
	$(CC) $(CFLAGS) -c src/disasm.c

asm: asm_main.o assembler.o Code.o Node.o opcode.o
	$(CC) $(CFLAGS) -o asm asm_main.o assembler.o Code.o Node.o opcode.o

asm_main.o: src/asm_main.c src/assembler.h
	$(CC) $(CFLAGS) -c src/asm_main.c

assembler.o: src/assembler.c src/assembler.h src/Code.h
	$(CC) $(CFLAGS) -c src/assembler.c

Code.o: src/Code.c src/Code.h src/Node.h
	$(CC) $(CFLAGS) -c src/Code.c

Node.o: src/Node.c src/Node.h src/opcode.h
	$(CC) $(CFLAGS) -c src/Node.c

clean:
	rm *.o
