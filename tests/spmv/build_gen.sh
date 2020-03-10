#!/bin/sh
riscv32-unknown-linux-gnu-g++ -O3 -fpermissive -I. -c -DGENERATE_ONLY  generate.c
riscv32-unknown-linux-gnu-g++ -O3 -fpermissive -I. -DGENERATE_ONLY  denseMV.c generate.o
