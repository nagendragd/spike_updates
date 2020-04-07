#!/bin/sh
riscv32-unknown-elf-g++ -O3 -march=rv32imafdc -I. -c -DGENERATE_ONLY  generate.c
riscv32-unknown-elf-g++ -O3 -march=rv32imafdc -I. -DGENERATE_ONLY  spMspM.c generate.o
