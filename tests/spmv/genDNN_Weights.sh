#!/bin/sh
./genSparseDNN 1024 1000 ./dnn/fc/mobilenet/mobileNet_w_1.txt  ./dnn/fc/mobileNet_weights.txt
./genSparseDNN 1280 1000 ./dnn/fc/mobilenet/mobileNetV2_w_1.txt  ./dnn/fc/mobileNetV2_weights.txt
./genSparseDNN 4096 1000 ./dnn/fc/vgg/vgg16_w_1.txt  ./dnn/fc/vgg16_weights.txt
./genSparseDNN 4096 1000 ./dnn/fc/vgg/vgg19_w_1.txt  ./dnn/fc/vgg19_weights.txt
./genSparseDNN 2048 1000 ./dnn/fc/resnet/resNet_w_1.txt  ./dnn/fc/resnet_weights.txt
./genSparseDNN 2048 1000 ./dnn/fc/resnet/resNetV2_w_1.txt  ./dnn/fc/resnetV2_weights.txt
./genSparseDNN 1024 1000 ./dnn/fc/densenet/denseNet_w_1.txt  ./dnn/fc/denseNet_weights.txt

