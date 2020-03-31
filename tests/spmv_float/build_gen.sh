#!/bin/sh
riscv32-unknown-elf-g++ -O3 -fpermissive -march=rv32imafdc -I. -c -DGENERATE_ONLY  generate.c
riscv32-unknown-elf-g++ -O3 -fpermissive -march=rv32imafdc -I. -DGENERATE_ONLY  spMVfloat.c generate.o
