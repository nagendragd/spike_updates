cd ../../build
./spike --isa=RV32IMAFDCV --varch=v256:e32:s256 -d ../../../pk/riscv-pk/build/pk ../tests/spmv/a.out 1 2 $1 0 1 
cd ../tests/spmv
