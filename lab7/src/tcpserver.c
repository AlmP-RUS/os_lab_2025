#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>

//#define SERV_PORT 10050
//#define BUFSIZE 100
#define SADDR struct sockaddr

#define ERR(a) { fprintf(stderr, a); exit(1); }

struct options
{
  uint16_t port;
  size_t   blocksize;
};

struct options handle_options(const int argc, char* const argv[]);

int main(const int argc, char* const argv[]) {
  struct options console_options = handle_options(argc, argv);

  const size_t kSize = sizeof(struct sockaddr_in);

  int lfd, cfd;
  int nread;
  char buf[console_options.blocksize];
  struct sockaddr_in servaddr;
  struct sockaddr_in cliaddr;

  if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }

  memset(&servaddr, 0, kSize);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(console_options.port);

  if (bind(lfd, (SADDR *)&servaddr, kSize) < 0) {
    perror("bind");
    exit(1);
  }

  if (listen(lfd, 5) < 0) {
    perror("listen");
    exit(1);
  }

  while (1) {
    unsigned int clilen = kSize;

    if ((cfd = accept(lfd, (SADDR *)&cliaddr, &clilen)) < 0) {
      perror("accept");
      exit(1);
    }
    printf("connection established\n");

    while ((nread = read(cfd, buf, console_options.blocksize)) > 0) {
      write(1, &buf, nread);
    }

    if (nread == -1) {
      perror("read");
      exit(1);
    }
    close(cfd);
  }
}

struct options handle_options(const int argc, char* const argv[])
{
  struct options return_value = { 0, 0 };
  bool port      = false,
       blocksize = false;
  for (;;)
  {
    static struct option long_options[] = 
    {
      { "port", required_argument, NULL, 0},
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
            return_value.port = (uint16_t) strtoul(optarg, NULL, 10);
            if (return_value.port < 0 || return_value.port > 65535)
            {
              ERR("Incorrect port value.\n");
            }
            port = true;
            break;
          case 1:
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
  if (!port || !blocksize)
  {
    ERR("A required argument is missing.\n");
  }
  return return_value;
}
