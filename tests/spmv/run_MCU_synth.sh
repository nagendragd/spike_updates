#!/bin/sh
for i in 64 128 256 512 1024 2048 4096
do
   for j in 10 20 30 40 50 60 70 80 90
   do
      denseVector_MCU_synth.sh ~/sim/spike/riscv-isa-sim/tests/spmv/synth/$i$j\_dense.dat >> dense_synth_ideal.txt

      sparseVector_MCU_synth.sh ~/sim/spike/riscv-isa-sim/tests/spmv/synth/$i$j\_dense.dat >> sparse_synth_ideal.txt

      helper_MCU_synth.sh ~/sim/spike/riscv-isa-sim/tests/spmv/synth/$i$j\_dense.dat >> helper_synth_ideal.txt
   done
done
