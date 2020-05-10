#!/bin/sh
rm a.out
riscv32-unknown-elf-g++ -O3 -fpermissive -I. -c -DGENERATE_ONLY  -DRISCV_BUILD generate.c
riscv32-unknown-elf-g++ -O3 -fpermissive -I. -DGENERATE_ONLY  -DRISCV_BUILD denseMV_scalar.c generate.o
