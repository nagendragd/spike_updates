#!/bin/sh
riscv32-unknown-elf-g++ -O3 -fpermissive -I. -c -DRISCV_BUILD generate.c
riscv32-unknown-elf-g++ -O3 -fpermissive -I. -DRISCV_BUILD denseMV.c generate.o
