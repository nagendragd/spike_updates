#!/bin/sh
riscv32-unknown-linux-gnu-g++ -O3 -fpermissive -I. -c generate.c
riscv32-unknown-linux-gnu-g++ -O3 -fpermissive -I. denseMV.c generate.o
