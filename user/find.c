#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void fmtname(char* path, char* file)
{
    char* p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; --p) {
    }
    ++p;

    if (strcmp(p, file) == 0) {
        printf("%s\n", path);
    }
}

void find(char* path, char* file)
{
    if (strcmp(path, "..") == 0) {
        return;
    }

    char          buf[512], *p;
    int           fd;
    struct dirent de;
    struct stat   st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
    case T_FILE:
        fmtname(path, file);
        break;

    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
            printf("ls: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0) {
                continue;
            }
            memmove(p, de.name, DIRSIZ);

            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
                continue;
            }

            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0) {
                printf("ls: cannot stat %s\n", buf);
                continue;
            }

            if (st.type == T_DIR) {
                find(buf, file);
            } else {
                fmtname(buf, file);
            }
        }
        break;
    }

    close(fd);
}

int main(int argc, char* argv[])
{
    char* path;
    char* file;
    if (argc < 2) {
        printf("Usage: find <path> <file> or find <file>\n");
        exit(0);
    } else if (argc == 2) {
        path = ".";
        file = argv[1];
    } else {
        path = argv[1];
        file = argv[2];
    }

    find(path, file);

    exit(0);
}