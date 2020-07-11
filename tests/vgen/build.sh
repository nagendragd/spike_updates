#!/bin/sh
g++ -O3 -fpermissive -I. -c generate.c
g++ -O3 -fpermissive -I. vgen.c generate.o
