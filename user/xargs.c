#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

#define nullptr 0
char buf[512];

char* readline()
{
    char c;
    int  pos = -1;
    // read one by one
    while (read(0, &c, 1)) {
        if (c == '\n') {
            break;
        } else {
            buf[++pos] = c;
        }
    }

    if (pos == -1) {
        return nullptr;
    }

    buf[++pos] = '\0';
    return buf;
}

int main(int argc, char* argv[])
{
    if (argc <= 1) {
        fprintf(2, "Usage: xargs command args... \n");
        exit(1);
    }

    char* arg[MAXARG];
    arg[0] = argv[1];
    for (int i = 2; i < argc; i++) {
        arg[i - 1] = argv[i];
    }

    char* r;
    while ((r = readline()) != nullptr) {
        arg[argc - 1] = r;
        if (fork() == 0) {  // child
            exec(argv[1], arg);
            // output error to stderr
            fprintf(2, "exec failed!\n");
            exit(1);
        } else {
            // parents
            // wait for child to quit
            wait(0);
        }
    }

    exit(0);
}
