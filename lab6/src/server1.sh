#!/bin/sh

make
LD_LIBRARY_PATH=./ ./server --port 20000 --tnum 4