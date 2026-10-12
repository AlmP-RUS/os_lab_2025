#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <limits.h>

//#define SERV_PORT 20001
//#define BUFSIZE 1024
#define SADDR struct sockaddr
#define SLEN sizeof(struct sockaddr_in)

#define ERR(a) { fprintf(stderr, a); exit(1); }

struct options
{
  uint16_t port;
  size_t   blocksize;
};

struct options handle_options(const int argc, char* const argv[]);

int main(int argc, char **argv) {
  struct options console_options = handle_options(argc, argv);

  int sockfd, n;
  char sendline[console_options.blocksize], recvline[console_options.blocksize + 1];
  struct sockaddr_in servaddr;

  if (argc - optind + 1 != 2) {
    printf("usage: client <IPaddress of server>\n");
    exit(1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(console_options.port);

  if (inet_pton(AF_INET, argv[optind - 1 + 1], &servaddr.sin_addr) < 0) {
    perror("inet_pton problem");
    exit(1);
  }
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket problem");
    exit(1);
  }

  write(1, "Enter string\n", 13);

  while ((n = read(0, sendline, console_options.blocksize)) > 0) {
    if (sendto(sockfd, sendline, n, 0, (SADDR *)&servaddr, SLEN) == -1) {
      perror("sendto problem");
      exit(1);
    }

    if (recvfrom(sockfd, recvline, console_options.blocksize, 0, NULL, NULL) == -1) {
      perror("recvfrom problem");
      exit(1);
    }

    printf("REPLY FROM SERVER= %s\n", recvline);
  }
  close(sockfd);
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
