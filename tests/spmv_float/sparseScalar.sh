#!/bin/sh
rm a.out
# Compile iteration #1
./build_gen.sh 
# Run spike iteration #1
cd ../../build
./spike --isa=RV32IMAFDCV --varch=v256:e32:s256 ../../../pk/riscv-pk/build/pk ../tests/spmv_float/a.out 0 2 $1 1 0 | grep "#define"  > ../tests/spmv_float/generated_macros.h
# Compile iteration #2
cd ../tests/spmv_float
./build.sh
# Run spike iteration #2
cd ../../build
./spike --isa=RV32IMAFDCV --varch=v256:e32:s256 ../../../pk/riscv-pk/build/pk ../tests/spmv_float/a.out 0 2 $1 1 0 | grep "#define"  > ../tests/spmv_float/generated_macros.h
# Compile iteration #3
cd ../tests/spmv_float
./build.sh
riscv32-unknown-elf-objdump -D a.out > a.dump
# Run spike iteration #3
cd ../../build
./spike --isa=RV32IMAFDCV --varch=v256:e32:s256 ../../../pk/riscv-pk/build/pk ../tests/spmv_float/a.out 1 2 $1 1 0  
cd ../tests/spmv_float
