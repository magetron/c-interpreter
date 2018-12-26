# c-interpreter

This is a small project where I try to make a C minimal interpreter in 10 days.

It's called Patrick's C compiler -> pcc.

## Files

* `pcc.c` - The source code of pcc.

* `hello.c` - A piece of testing C code that outputs `Hello World!\n`

* `fibonacci.c` - A piece of testing C code that outputs fibonacci sequence using recursion.

## Usage

`gcc -m32 pcc.c -o pcc`

`-m32` tag is used for `64-bit` machines.

`./pcc hello.c`

### Bootstrap

pcc is a interepter that bootstraps. Therefore, feel free to execute commands like :

`./pcc pcc.c fibonacci.c`

Although it is not recommended since this slows down the program for no reason.

### Options

`./pcc -s` outputs ASM code for VM / Further compiler purposes.

`./pcc -d` outputs DEBUG information.

