#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <limits.h>

#define ERR(a) { fprintf(stderr, a); exit(1); }

//#define BUFSIZE 100
#define SADDR struct sockaddr
#define SIZE sizeof(struct sockaddr_in)

struct options
{
  size_t   blocksize;
};

struct options handle_options(const int argc, char* const argv[]);

int main(int argc, char *argv[]) {
  struct options console_options = handle_options(argc, argv);

  int fd;
  int nread;
  char buf[console_options.blocksize];
  struct sockaddr_in servaddr;
  if (argc - optind + 1 < 3) {
    printf("Too few arguments \n");
    exit(1);
  }

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creating");
    exit(1);
  }

  memset(&servaddr, 0, SIZE);
  servaddr.sin_family = AF_INET;

  if (inet_pton(AF_INET, argv[optind - 1 + 1], &servaddr.sin_addr) <= 0) {
    perror("bad address");
    exit(1);
  }

  servaddr.sin_port = htons(atoi(argv[optind - 1 + 2]));

  if (connect(fd, (SADDR *)&servaddr, SIZE) < 0) {
    perror("connect");
    exit(1);
  }

  write(1, "Input message to send\n", 22);
  while ((nread = read(0, buf, console_options.blocksize)) > 0) {
    if (write(fd, buf, nread) < 0) {
      perror("write");
      exit(1);
    }
  }

  close(fd);
  exit(0);
}

struct options handle_options(const int argc, char* const argv[])
{
  struct options return_value = { 0 };
  bool blocksize = false;
  for (;;)
  {
    static struct option long_options[] = 
    {
      { "blocksize", required_argument, NULL, 0},
      { 0, 0, 0, 0 }
    };
    int option_index = -1;
    char c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1)
    {
      break;
    }
    switch(c)
    {
      case 0:
        switch (option_index)
        {
          case -1:
            ERR("An error occured while handling console options.\n");
          case 0:
            return_value.blocksize = (size_t) strtoul(optarg, NULL, 10);
            if (return_value.blocksize == ULONG_MAX)
            {
              ERR("Incorrect blocksize value.\n");
            }
            blocksize = true;
            break;
        }
        break;
        case '?':
          fprintf(stderr, "Unrecognized option.\n");
          break;
    }
  }
  if (!blocksize)
  {
    ERR("A required argument is missing.\n");
  }
  return return_value;
}
