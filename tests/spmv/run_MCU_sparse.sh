#!/bin/sh
for i in 64 128 256 512 1024 2048 4096
do
   for j in 10 20 30 40 50 60 70 80 90
   do
      sparseVector_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/synth/$i$j\_dense.dat >> sparse_synth_ideal.txt
   done
done
