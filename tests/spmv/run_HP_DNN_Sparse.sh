#!/bin/sh
sparseVector_HP_V8.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/denseNet_weights.txt >& dense_sparse_dnn_hp_vector.txt
sparseVector_HP_V8.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/resnet_weights.txt >& resnet_sparse_dnn_hp_vector.txt
sparseVector_HP_V8.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/mobileNetV2_weights.txt  >& mnetv2_sparse_dnn_hp_vector.txt
sparseVector_HP_V8.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/vgg16_weights.txt>& vgg16_sparse_dnn_hp_vector.txt
sparseVector_HP_V8.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/mobileNet_weights.txt  >& mnet_sparse_dnn_hp_vector.txt  
sparseVector_HP_V8.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/vgg19_weights.txt>& vgg19_sparse_dnn_hp_vector.txt
sparseVector_HP_V8.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/resnetV2_weights.txt >& resnetv2_sparse_dnn_hp_vector.txt

