#!/bin/bash

#number of threads:
for t in 1 2 4 6 8 12 16
do
  for n in 1 10 100 1000 10000 100000 500000 1000000
  do
     g++   project1.cpp  -DNUMT=$t -DNUMTRIALS=$n  -o project1  -lm  -fopenmp
    ./project1
  done
done