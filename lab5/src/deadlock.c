#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;

void *thread_foo_1(void* arg)
{
    pthread_mutex_lock(&mutex_1);
    sleep(1);
    pthread_mutex_lock(&mutex_2);
    
    pthread_mutex_unlock(&mutex_1);
    pthread_mutex_unlock(&mutex_2);
}

void *thread_foo_2(void* arg)
{
    pthread_mutex_lock(&mutex_2);
    sleep(1);
    pthread_mutex_lock(&mutex_1);
    
    pthread_mutex_unlock(&mutex_2);
    pthread_mutex_unlock(&mutex_1);
}

int main()
{
    pthread_t thread_1, thread_2;

    pthread_create(&thread_1, NULL, thread_foo_1, NULL);
    pthread_create(&thread_2, NULL, thread_foo_2, NULL);

    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);
    
    return 0;
}