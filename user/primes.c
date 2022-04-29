#include "kernel/types.h"
#include "user/user.h"

void primes(int p[2])
{
    close(p[1]);
    int buf;
    if (!read(p[0], &buf, 4)) {
        close(p[0]);
        exit(0);
    }
    int x = buf;
    printf("prime %d\n", x);

    int p1[2];
    pipe(p1);

    int pid = fork();
    if (pid != 0) {
        // parent
        close(p1[0]);
        while (read(p[0], &buf, 4)) {
            if (buf % x != 0) {
                write(p1[1], &buf, 4);
            }
        }
        close(p1[1]);
        wait(0);
    } else {
        primes(p1);
    }
}

// use fork and pipe to get primes
int main(int argc, int* argv[])
{
    int x = 2;
    int p[2];
    pipe(p);

    int pid = fork();
    if (pid != 0) {
        // parent
        close(p[0]);
        printf("prime %d\n", x);
        for (int i = 3; i <= 35; ++i) {
            if (i % x != 0) {
                write(p[1], &i, 4);
            }
        }
        write(p[1], 0, 0);
        close(p[1]);
        wait(0);
    } else {
        // child
        primes(p);
    }

    exit(0);
}