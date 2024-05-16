#!/bin/bash

./cp $1 -S prog.asm
nasm -felf64 prog.asm -o prog.o
nasm -felf64 compiler/lib_no_libc.asm -o lib.o
ld --strip-all prog.o lib.o -o prog.out

rm -f lib.o
rm -f prog.o