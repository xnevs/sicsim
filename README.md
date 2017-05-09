# sicsim
A simple assembler and emulator for the SIC/XE hypothetical computer.

## Building

Run `make` in the root directory.

The result of building are three binaries `asm`, `sim` and `gsim`.

GTK+ is required for building the graphical emulator `gsim`.

## Assembler

The `asm` binary is the assembler that takes a `.asm` file (e.g., `examples/hanoi.asm`) as the only argument and produces an object `.obj` file.

## Emulator

The `sim` binary is the command line SIC/XE emulator. After running `./sim` type `help` to learn more.

The `gsim` binary is the graphical SIC/XE emulator.

![screenshot](/gsim.png?raw=true "gsim screenshot")
