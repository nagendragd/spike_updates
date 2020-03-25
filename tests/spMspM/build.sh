#!/bin/sh
riscv32-unknown-elf-g++ -O3 -fpermissive -I. -c generate.c
riscv32-unknown-elf-g++ -O3 -fpermissive -I. gustavson.c generate.o
