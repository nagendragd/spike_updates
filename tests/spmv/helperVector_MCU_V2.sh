#!/bin/sh
rm a.out
# Compile iteration #1
./build_helper_scalar_gen.sh 
# Run spike iteration #1
cd ../../build
./spike --isa=RV32IMAFDCV --varch=v64:e32:s64 --timing=timings.txt ../../../pk/riscv-pk/build/pk ../tests/spmv/a.out 0 3 $1 1 2 2 | grep "#define"  > ../tests/spmv/generated_macros.h
# Compile iteration #2
cd ../tests/spmv
./build_helper_scalar.sh
# Run spike iteration #2
cd ../../build
./spike --isa=RV32IMAFDCV --varch=v64:e32:s64 --timing=timings.txt ../../../pk/riscv-pk/build/pk ../tests/spmv/a.out 0 3 $1 1 2 2 | grep "#define"  > ../tests/spmv/generated_macros.h
# Compile iteration #3
cd ../tests/spmv
./build_helper_scalar.sh
riscv32-unknown-elf-objdump -D a.out > a.dump
# Run spike iteration #3
cd ../../build
./spike --isa=RV32IMAFDCV --varch=v64:e32:s64 --timing=timings.txt ../../../pk/riscv-pk/build/pk ../tests/spmv/a.out 1 3 $1 1 2 2  
cd ../tests/spmv