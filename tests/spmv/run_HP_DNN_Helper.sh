#!/bin/sh
helperVector_HP_V8.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/denseNet_weights.txt >& dense_helper_dnn_hp_vector.txt
helperVector_HP_V8.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/resnet_weights.txt >& resnet_helper_dnn_hp_vector.txt
helperVector_HP_V8.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/mobileNetV2_weights.txt  >& mnetv2_helper_dnn_hp_vector.txt
helperVector_HP_V8.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/vgg16_weights.txt>& vgg16_helper_dnn_hp_vector.txt
helperVector_HP_V8.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/mobileNet_weights.txt  >& mnet_helper_dnn_hp_vector.txt  
helperVector_HP_V8.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/vgg19_weights.txt>& vgg19_helper_dnn_hp_vector.txt
helperVector_HP_V8.sh ~/sim/spike/riscv-isa-sim/tests/spmv/dnn/fc/resnetV2_weights.txt >& resnetv2_helper_dnn_hp_vector.txt

