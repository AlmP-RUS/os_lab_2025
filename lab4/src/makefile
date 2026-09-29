CC=gcc
CFLAGS=-I. -Wall -Wpedantic -L.

all: parallel_min_max process_memory

1: parallel_min_max

3: process_memory

5: parallel_sum

5-lib: parallel_sum_lib_main

parallel_sum_lib_main: parallel_sum_lib_lib.h libparallel_sum_lib_lib.a parallel_sum_lib_main.c
	$(CC) -o parallel_sum_lib_main parallel_sum_lib_main.c -lparallel_sum_lib_lib -lpthread $(CFLAGS)

libparallel_sum_lib_lib.a: parallel_sum_lib_lib.h parallel_sum_lib_lib.c utils.o
	$(CC) -c -o parallel_sum_lib_lib.o parallel_sum_lib_lib.c -lpthread $(CFLAGS)
	ar rcs libparallel_sum_lib_lib.a parallel_sum_lib_lib.o utils.o
	rm -f parallel_sum_lib_lib.o utils.o

parallel_sum: parallel_sum.c utils.o
	$(CC) -o parallel_sum parallel_sum.c utils.o -lpthread $(CFLAGS)

process_memory: process_memory.c
	$(CC) -o process_memory process_memory.c 

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h parallel_min_max.c
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

clean :
	rm -f utils.o find_min_max.o parallel_min_max process_memory parallel_sum libparallel_sum_lib_lib.a parallel_sum_lib_main
