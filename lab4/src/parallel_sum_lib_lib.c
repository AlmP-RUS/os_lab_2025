#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "utils.h"
#include "parallel_sum_lib_lib.h"

struct SumArgs {
  int *array;
  uint32_t begin;
  uint32_t end;
};

uint32_t Sum(const struct SumArgs *args) {
  uint32_t sum = 0;
  for (uint32_t i = args->begin; i <= args->end; i++)
  {
    sum += args->array[i];
  }
  return sum;
}

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  static uint64_t returnValue;
  returnValue = (uint64_t)Sum(sum_args);
  return (void *)&returnValue;
}

int parallel_sum_lib_lib(uint32_t threads_num, uint32_t array_size, uint32_t seed) {
  pthread_t threads[threads_num];

  int *array = malloc(sizeof(int) * array_size);

  GenerateArray(array, array_size, seed);

  struct timespec begin, end;
  clock_gettime(CLOCK_MONOTONIC_RAW, &begin);

  struct SumArgs args[threads_num];
  for (uint32_t i = 0; i < threads_num; i++) {
    args[i].array = array;
    args[i].begin = (uint32_t) ((uint64_t) array_size * i        / threads_num);
    args[i].end   = (uint32_t) ((uint64_t) array_size * (i + 1)  / threads_num - 1);
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  uint64_t total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    uint64_t *returnValue;
    pthread_join(threads[i], (void **)&returnValue);
    total_sum += *returnValue;
  }

  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    
  printf (
    "Total time = %.2e seconds\n",
    (end.tv_nsec - begin.tv_nsec) / 1000000000.0 + (end.tv_sec  - begin.tv_sec));

  free(array);
  printf("Total sum  = %ld\n", total_sum);
  return 0;
}
