#!/bin/sh
for i in 64 128 256 512 1024 2048 4096
do
   for j in 10 20 30 40 50 60 70 80 90
   do
     ./gen_rand 2 $i $j $i$j.h $i$j
   done
done
