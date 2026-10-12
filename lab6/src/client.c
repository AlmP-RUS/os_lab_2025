#define _GNU_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <pthread.h>

struct Server {
  char ip[255];
  int port;
};

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod);
bool ConvertStringToUI64(const char *str, uint64_t *val);

void handle_console_options(const int argc, char* const argv[], uint64_t* k, uint64_t* mod, char servers[]);
void process_server_addresses(char servers[], struct Server *to[], uint64_t* servers_num);

void create_sockets(struct Server const to[], int *sck, uint64_t servers_num);

void parallel_servers_communication(int* const sck, uint64_t servers_num, uint64_t k, uint64_t mod);

int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers[255] = {'\0'}; // TODO: explain why 255

  handle_console_options(argc, argv, &k, &mod, servers);

  struct Server* to = NULL;
  uint64_t servers_num;
  process_server_addresses(servers, &to, &servers_num);

  int *sck = (int*) alloca(servers_num * sizeof(int));
  create_sockets(to, sck, servers_num);

  parallel_servers_communication(sck, servers_num, k, mod);

  free(to);

  return 0;
}

void handle_console_options(const int argc, char* const argv[], uint64_t* k, uint64_t* mod, char servers[])
{
  while (true) {
    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        ConvertStringToUI64(optarg, k);
        // TODO: your code here
        if (*k <= 0)
        {
          fprintf(stderr, "k can't be <= 0.\n");
          exit(1);
        }
        break;
      case 1:
        ConvertStringToUI64(optarg, mod);
        // TODO: your code here
        if (*mod <= 0)
        {
          fprintf(stderr, "mod can't be <= 0.\n");
          exit(1);
        }
        break;
      case 2:
        // TODO: your code here
        if (strlen(optarg) == 0 || strlen(optarg) >= 255)
        {
          printf("Invalid filename.\n");
          exit(1);
        }
        memcpy(servers, optarg, strlen(optarg));
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (*k == -1 || *mod == -1 || !strlen(servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    exit(1);
  }
}

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
  uint64_t result = 0;
  a = a % mod;
  while (b > 0) {
    if (b % 2 == 1)
      result = (result + a) % mod;
    a = (a * 2) % mod;
    b /= 2;
  }

  return result % mod;
}

bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}

// This function dynamically allocates to. This needs to be freed.
void process_server_addresses(char servers[], struct Server *to[], uint64_t* servers_num)
{
  FILE* servers_list = fopen(servers, "r");
  if (servers_list == NULL)
  {
    fprintf(stderr, "File do not exist.\n");
    exit(1);
  }

  *servers_num = 0;
  char* str = NULL;
  size_t str_len = 0;
  while (getline(&str, &str_len, servers_list) != -1)
  {
    (*servers_num)++;    
  }
  free(str);
  str = NULL;

  *to = malloc(sizeof(struct Server) * *servers_num);
  rewind(servers_list);

  int i = 0;
  while (getline(&str, &str_len, servers_list) != -1)
  {
    // strdupa allocate new string on the stack
    char *str_dup = strdupa(str);
    char *ip, *port;

    ip   = strsep(&str_dup, ":\n");
    port = strsep(&str_dup, ":\n");
    if (ip == NULL || port == NULL)
    {
      fprintf(stderr, "Incorrect server address: %s.\n", str);
      free(str);
      free(to);
      exit(1);
    }
    ip   = strdup(ip);
    port = strdup(port);

    (*to)[i].port = strtol(port, NULL, 10);
    if ((*to)[i].port < 0 || (*to)[i].port > 65535)
    {
      fprintf(stderr, "Incorrect server port: %s:%s", ip, port);
      free(str);
      free(port);
      free(ip);
      free(*to);
      exit(1);
    }
    if (strlen(ip) <= 0 || strlen(ip) > (255 - 1))
    {
      fprintf(stderr, "ip value can't be longer then 254 characters: %s:%s", ip, port);
      free(str);
      free(port);
      free(ip);
      free(*to);
      exit(1);
    }
    memcpy((*to)[i].ip, ip, strlen(ip) + 1);
    free(ip);
    free(port);
    i++;
  }
  free(str);
}

void create_sockets(struct Server const to[], int *sck, uint64_t servers_num)
{
  for (int i = 0; i < servers_num; i++) {
    struct hostent *hostname = gethostbyname(to[i].ip);

    if (hostname == NULL) {
      fprintf(stderr, "gethostbyname failed with %s\n", to[i].ip);
      exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(to[i].port);
    server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr_list[0]);

    sck[i] = socket(AF_INET, SOCK_STREAM, 0);
    if (sck[i] < 0) {
      fprintf(stderr, "Socket creation failed!\n");
      exit(1);
    }

    if (connect(sck[i], (struct sockaddr *)&server, sizeof(server)) < 0) {
      fprintf(stderr, "Connection failed to %s:%d\n", to[i].ip, to[i].port);
      exit(1);
    }
  }
}

struct data_for_thread
{
  uint64_t k;
  uint64_t mod;
  int sck;
  int i;
};

void *threads_foo(void *data)
{
  uint64_t k   = ((struct data_for_thread*) data) -> k;
  uint64_t mod = ((struct data_for_thread*) data) -> mod;
  int      sck = ((struct data_for_thread*) data) -> sck;

  printf("I'm the thread no. %d\n", ((struct data_for_thread*) data) -> i);

  uint64_t begin = 1;
  uint64_t end = k;

  char task[sizeof(uint64_t) * 3];
  memcpy(task, &begin, sizeof(uint64_t));
  memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
  memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

  if (send(sck, task, sizeof(task), 0) < 0) {
    fprintf(stderr, "Send failed\n");
    exit(1);
  }

  char response[sizeof(uint64_t)];
  if (recv(sck, response, sizeof(response), 0) < 0) {
    fprintf(stderr, "Recieve failed\n");
    exit(1);
  }

  // TODO: from one server
  // unite results
  uint64_t answer = 0;
  memcpy(&answer, response, sizeof(uint64_t));
  printf("answer: %lu\n", answer);

  close(sck);

  return NULL;
}

void parallel_servers_communication(int* const sck, uint64_t servers_num, uint64_t k, uint64_t mod)
{
  pthread_t* threads = alloca(servers_num * sizeof(pthread_t));
  struct data_for_thread* data = alloca(servers_num * sizeof(struct data_for_thread));
  for (int i = 0; i < servers_num; i++)
  {
    data[i].k = k;
    data[i].mod = mod;
    data[i].sck = sck[i];
    data[i].i = i;
    pthread_create(&threads[i], NULL, threads_foo, (void*)&data[i]);
  }

  for (int i = 0; i < servers_num; i++)
  {
    pthread_join(threads[i], NULL);
  }
}
