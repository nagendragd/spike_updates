#!/bin/sh
riscv32-unknown-elf-g++ -O3 -fpermissive -I. -c -DGENERATE_ONLY  -DRISCV_BUILD generate.c
riscv32-unknown-elf-g++ -O3 -fpermissive -I. -DGENERATE_ONLY  -DRISCV_BUILD denseMV.c generate.o
