#!/bin/sh
denseVector_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/denseNet_weights.txt > dense_dnn_ideal.txt
denseVector_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/resnet_weights.txt >> dense_dnn_ideal.txt
denseVector_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/mobileNetV2_weights.txt  >> dense_dnn_ideal.txt
denseVector_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/vgg16_weights.txt>> dense_dnn_ideal.txt
denseVector_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/mobileNet_weights.txt  >> dense_dnn_ideal.txt  
denseVector_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/vgg19_weights.txt>> dense_dnn_ideal.txt
denseVector_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/resnetV2_weights.txt >> dense_dnn_ideal.txt

sparseVector_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/denseNet_weights.txt > sparse_dnn_ideal.txt
sparseVector_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/resnet_weights.txt >> sparse_dnn_ideal.txt
sparseVector_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/mobileNetV2_weights.txt  >> sparse_dnn_ideal.txt
sparseVector_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/vgg16_weights.txt>> sparse_dnn_ideal.txt
sparseVector_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/mobileNet_weights.txt  >> sparse_dnn_ideal.txt  
sparseVector_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/vgg19_weights.txt>> sparse_dnn_ideal.txt
sparseVector_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/resnetV2_weights.txt >> sparse_dnn_ideal.txt

helper_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/denseNet_weights.txt > helper_dnn_ideal.txt
helper_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/resnet_weights.txt >> helper_dnn_ideal.txt
helper_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/mobileNetV2_weights.txt  >> helper_dnn_ideal.txt
helper_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/vgg16_weights.txt>> helper_dnn_ideal.txt
helper_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/mobileNet_weights.txt  >> helper_dnn_ideal.txt  
helper_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/vgg19_weights.txt>> helper_dnn_ideal.txt
helper_MCU.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/resnetV2_weights.txt >> helper_dnn_ideal.txt
