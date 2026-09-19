#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            // error handling
            if (seed <= 0) {
              printf("seed is a positive number\n");
              return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            // error handling
            if (array_size <= 0) {
              printf("array_size is a positive number\n");
              return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            // error handling
            if (pnum <= 0) {
              printf("pnum is a positive number grater than zero\n");
              return 1;
            }
            break;
          case 3:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int fd[2];
  if (pipe(fd) != 0) {
    printf("Failed to create pipe.\n");
    return 1;
  }

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        //printf("%d %d\n", array_size / pnum * (active_child_processes - 1), array_size / pnum * (active_child_processes));
        struct MinMax min_max = GetMinMax(array, array_size / pnum * (active_child_processes - 1), array_size / pnum * (active_child_processes));
        if (with_files) {
          // use files here
          FILE* file = fopen("sync", "ab");
          if (file == NULL)
          {
            printf("Unable to sync via files\n");
            return 1;
          }
          else
          {
            printf("file opend\n");
          }
          fwrite(&min_max, sizeof(struct MinMax), 1, file);
          fclose(file);
        } else {
          write(fd[1], &min_max, sizeof(struct MinMax));
          //printf("C - %d %d\n", min_max.min, min_max.max);
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  while (wait(NULL) > 0);

    struct MinMax* min_max_part = malloc(pnum * sizeof(struct MinMax));
    if (min_max_part == NULL)
    {
      printf("Failed to syncronize forks\n");
      return 1;
    }
    if (with_files) {
      // read from files
      FILE* file = fopen("sync", "rb");
      if (file == NULL)
      {
        printf("Failed to sync via files\n");
        return 1;
      }
      fread(min_max_part, sizeof(struct MinMax), pnum, file);
      fclose(file);
      if (remove("sync") != 0)
      {
        printf("Unable to delete a file\n");
        return 1;
      }
    } else {
      // read from pipes
      read(fd[0], min_max_part, sizeof(struct MinMax) * pnum);
    }
    for (int i = 0; i < pnum; i++)
    {
      //printf("P - %d %d\n", min_max_part[i].min, min_max_part[0].max);
      min_max.min = (min_max.min > min_max_part[i].min) ? min_max_part[i].min : min_max.min;
      min_max.max = (min_max.max < min_max_part[i].max) ? min_max_part[i].max : min_max.max;
    }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
