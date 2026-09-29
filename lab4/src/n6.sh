#!/bin/sh

make 5 > /dev/null
echo ">>>Normal<<<"
./parallel_sum --threads_num 1 --seed 2 --array_size 100000000000

make 5-lib > /dev/null
echo ">>>Library<<<"
./parallel_sum_lib_main --threads_num 1 --seed 2 --array_size 100000000000

make clean > /dev/null