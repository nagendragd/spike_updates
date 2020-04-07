#!/bin/sh
riscv32-unknown-elf-g++ -O3 -march=rv32imafdc -I. -c generate.c
riscv32-unknown-elf-g++ -O3 -march=rv32imafdc -I. spMspM.c generate.o
