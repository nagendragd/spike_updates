#!/bin/sh
riscv32-unknown-elf-g++ -fpermissive -march=rv32imafdc -I. -c generate.c
riscv32-unknown-elf-g++ -fpermissive -march=rv32imafdc -I. spMVfloat.c generate.o
