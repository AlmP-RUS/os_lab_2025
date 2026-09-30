#!/bin/sh

make mutex_on mutex_off > /dev/null

echo ">>>mutex off<<<"
./mutex_off

echo "\n>>>mutex on<<<"
./mutex_on

make clean > /dev/null