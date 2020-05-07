#!/bin/sh
rm a.out
# Compile iteration #1
./build_helper_scalar_gen.sh 
# Run spike iteration #1
cd ../../build
./spike --isa=RV32IMAFDCV --varch=v128:e32:s128 ../../../pk/riscv-pk/build/pk ../tests/spmv/a.out 0 2 $1 1 1 4 | grep "#define"  > ../tests/spmv/generated_macros.h
# Compile iteration #2
cd ../tests/spmv
./build_helper_scalar.sh
# Run spike iteration #2
cd ../../build
./spike --isa=RV32IMAFDCV --varch=v128:e32:s128 ../../../pk/riscv-pk/build/pk ../tests/spmv/a.out 0 2 $1 1 1 4 | grep "#define"  > ../tests/spmv/generated_macros.h
# Compile iteration #3
cd ../tests/spmv
./build_helper_scalar.sh
riscv32-unknown-elf-objdump -D a.out > a.dump
# Run spike iteration #3
cd ../../build
./spike --isa=RV32IMAFDCV --varch=v128:e32:s128 ../../../pk/riscv-pk/build/pk ../tests/spmv/a.out 1 2 $1 1 1 4  
cd ../tests/spmv
