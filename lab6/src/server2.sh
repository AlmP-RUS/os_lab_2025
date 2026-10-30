#!/bin/sh

make
LD_LIBRARY_PATH=./ ./server --port 20001 --tnum 15
