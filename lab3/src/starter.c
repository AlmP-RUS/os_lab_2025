#include <unistd.h>

int main(int argc, char* argv[])
{
    execvp("./sequential_min_max", argv);
    return 0;
}