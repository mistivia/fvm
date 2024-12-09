# Fantasy Virtual Machine

A 64-bit stack-based virtual machine and its assembler, created for fun.

## Build

FVM includes two parts: the virtual machine and the assembler.

The build the assembler, you need to intall CHICKEN Scheme first.

For example, on Arch Linux:

    sudo pacman -S chicken

Then install CHICKEN Scheme dependencies:

    sudo chicken-install matchable srfi-69 srfi-4

Then run make, which will build both components:

    make

If successful, you will see two executables, the virtual machine `fvm` and the assembler `fvm-as`.

## Run

After writing the assembly code (examples can be found in the `examples/` directory), first run the assembler:

    ./fvm-as input.asm bytecode.bin

This will generate the bytecode file `output.bin`. Then run the virtual machine:

    ./fvm bytecode.bin

## Registers

FVM contains 4 registers:

- SP: stack pointer
- BP：base pointer
- PC：program counter
- RV: returned value

There are no general-purpose registers; all operations happen on the stack.

## Opcodes

All opcodes are single-byte, except for immediate value instructions.

See `src/fvm.h` for a full list of opcodes.
