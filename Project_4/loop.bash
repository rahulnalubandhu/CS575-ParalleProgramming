#!/bin/bash

#number of threads:
for t in 1 2 4 6 8 12 14 16
do
  for n in 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608
  do
      g++   proj04.cpp  -DNUMT=$t -DARRAYSIZE=$n  -o proj04  -lm  -fopenmp
    ./proj04
  done
done


# #number of threads:
# for t in 1 2 4 6 8 12 16
# do
#   for n in 1 10 100 1000 10000 100000 500000 1000000
#   do
#      g++   project1.cpp  -DNUMT=$t -DNUMTRIALS=$n  -o project1  -lm  -fopenmp
#     ./project1
#   done
# done