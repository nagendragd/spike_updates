#!/bin/sh
rm tamu_log.txt
for f in ~/sim/spike/riscv-isa-sim/tests/spmv/tamu/*.mtx
do
    echo "Processing $f"
    helper_tamu.sh $f | grep "Took" >> tamu_log.txt
done
