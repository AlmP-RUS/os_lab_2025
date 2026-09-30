#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <pthread.h>

#include <unistd.h>
#include <getopt.h>

#define ERROR(a) { printf("%s\n", a); exit(1); }

uint32_t k, pnum, mod;
uint64_t result = 1;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct params
{
    uint32_t start;
    uint32_t end;
};

void *thread_calc(void *opt);

void handle_options(const int argc, char *const argv[], uint32_t* k, uint32_t* pnum, uint32_t* mod);

int main(const int argc, char *const argv[])
{
    handle_options(argc, argv, &k, &pnum, &mod);

    struct params   threads_params[pnum];
    pthread_t       threads[pnum];

    for (int i = 0; i < pnum; i++)
    {
        threads_params[i].start = k * i         / pnum + 1;
        threads_params[i].end   = k * (i + 1)   / pnum;
        pthread_create(&threads[i], NULL, thread_calc, (void*) &threads_params[i]);
    }

    for (int i = 0; i < pnum; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("result: %llu\n", result);

    return 0;
}

void handle_options(const int argc, char *const argv[], uint32_t* k, uint32_t* pnum, uint32_t* mod)
{
    if (k == NULL || pnum == NULL || mod == NULL)
    {
        ERROR("Misuse of handle_options func.")
    }

    *k = 0; *pnum = 0; *mod = 0;

    for(;;)
    {
        static struct option long_options[] = {
            {"pnum",    required_argument, NULL, 0},
            {"mod",     required_argument, NULL, 0},
            {0, 0, 0, 0}
        };

        int option_index;
        int c = getopt_long(argc, argv, "k:", long_options, &option_index);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 0:
                switch (option_index)
                {
                    case 0:
                        *pnum = strtoul(optarg, NULL, 10);
                        if (*pnum == 0)
                        {
                            ERROR("pnum must be > 0.");
                        }
                        break;
                    
                    case 1:
                        *mod = strtoul(optarg, NULL, 10);
                        if (*mod == 0)
                        {
                            ERROR("mod must be > 0.");
                        }
                        break;
                }
                break;
            case 'k':
                *k = strtoul(optarg, NULL, 10);
                if (*k == 0)
                {
                    ERROR("k must be > 0.");
                }
                break;
            case '?':
                break;
        }
    }

    if (*k == 0 || *pnum == 0 || *mod == 0)
    {
        ERROR("Usage: ./factorial -k \"num\" --pnum \"num\" --mod \"num\"");
    }
}

void *thread_calc(void *opt)
{
    uint64_t mid_result = 1;

    struct params thread_params = *(struct params *) opt;
   
    for (int i = thread_params.start; i <= thread_params.end; i++)
    {
        mid_result = (mid_result * i) % mod;
    }

    pthread_mutex_lock(&mutex);
    result = (result * mid_result) % mod;
    pthread_mutex_unlock(&mutex);
}