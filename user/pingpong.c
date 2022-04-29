#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    int p[2];
    pipe(p);

    int pid;
    if ((pid = fork()) == 0) {
        // child
        char buf[2];
        read(p[0], buf, 1);
        close(p[0]);

        printf("%d: received ping\n", getpid());

        write(p[1], buf, 1);
        close(p[1]);
    } else {
        // parent
        char buf[2] = "s";
        write(p[1], buf, 1);
        close(p[1]);
        // wait for child quit
        wait(0);

        read(p[0], buf, 1);
        close(p[0]);
        printf("%d: received pong\n", getpid());
    }

    exit(0);
}