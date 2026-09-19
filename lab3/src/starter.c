#include <unistd.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    if (fork() == 0)
    {
        execvp("./sequential_min_max", argv);
    }
    printf("Starter\n");
    return 0;
}