# c-interpreter

This is a small project where I try to make a C minimal interpreter in 10 days.

It's called Patrick's C compiler -> pcc.

## Files

* `pcc.c` - The source code of pcc.

* `hello.c` - A piece of testing C code that outputs `Hello World!\n`

## Usage

`gcc -m32 pcc.c -o pcc`

`-m32` tag is used for `64-bit` machines.

`./pcc hello.c`

