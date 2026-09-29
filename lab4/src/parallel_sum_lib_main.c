#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include "parallel_sum_lib_lib.h"

int main(int argc, char **argv) {
  uint32_t threads_num  = 0;
  uint32_t array_size   = 0;
  uint32_t seed         = 0;

  static struct option long_options[] = {
      {"threads_num", required_argument, NULL, 0},
      {"seed",        required_argument, NULL, 0},
      {"array_size",  required_argument, NULL, 0},
      {0, 0, 0, 0}
    };
  
  for (;;) {
    int option_index = 0;

    int c = getopt_long(argc, argv, "", long_options, &option_index);

    if (c == -1) {
      break;
    }

    if (c == 0) {
      switch (option_index) {
        case 0:
          threads_num = strtol(optarg, NULL, 10);
          if (threads_num == 0) {
            printf("threads_num must be > 0.\n");
            return 1;
          }
          break;

        case 1:
          seed = strtol(optarg, NULL, 10);
          if (seed == 0) {
            printf("seed must be > 0.\n");
            return 1;
          }
          break;

        case 2:
          array_size = strtol(optarg, NULL, 10);
          if (array_size == 0) {
            printf("array_size must be > 0.\n");
            return 1;
          }
          break;
      }
    }
  }

  if (threads_num == 0 || seed == 0 || array_size == 0) {
    printf(
      "Usage: %s --%s \"num\" --%s \"num\" --%s \"num\"\n",
      argv[0],
      long_options[0].name,
      long_options[1].name,
      long_options[2].name);
    return 1;
  }

  return parallel_sum_lib_lib(threads_num, array_size, seed);
}
