#!/bin/sh
rm a.out
# Compile iteration #1
./build_gen.sh 
# Run spike iteration #1
cd ../../build
./spike --isa=RV32IMCV --varch=v256:e32:s256 ../../../pk/riscv-pk/build/pk ../tests/spmv/a.out 0 2 /home/ndg0068/sim/spike/riscv-isa-sim/tests/spmv/tamu/1138_bus.mtx 1 2 | grep "#define"  > ../tests/spmv/generated_macros.h
# Compile iteration #2
cd ../tests/spmv
./build.sh
# Run spike iteration #2
cd ../../build
./spike --isa=RV32IMCV --varch=v256:e32:s256 ../../../pk/riscv-pk/build/pk ../tests/spmv/a.out 0 2 /home/ndg0068/sim/spike/riscv-isa-sim/tests/spmv/tamu/1138_bus.mtx 1 2 | grep "#define"  > ../tests/spmv/generated_macros.h
# Compile iteration #3
cd ../tests/spmv
./build.sh
riscv32-unknown-elf-objdump -D a.out > a.dump
# Run spike iteration #3
cd ../../build
./spike --isa=RV32IMCV --varch=v256:e32:s256 ../../../pk/riscv-pk/build/pk ../tests/spmv/a.out 1 2 /home/ndg0068/sim/spike/riscv-isa-sim/tests/spmv/tamu/1138_bus.mtx 1 2  
cd ../tests/spmv