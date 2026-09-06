#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    if(argc < 2){
        fprintf(2, "usage: xargs command [args...]\n");
        exit(1);
    }

    char *cmd[MAXARG];
    int fixed = argc - 1;
    for (int i = 0 ; i < fixed ; i++) {
        cmd[i] = argv[i+1];
    }

    char line[512];
    int n = 0;
    char c;
    while(read(0, &c, 1) == 1) {
        if (c == '\n') {
            line[n] = 0;
            cmd[fixed] = line;
            cmd[fixed+1] = 0;
            int pid = fork();
            if (pid == 0) {
                exec(argv[1], cmd);
                fprintf(2, "xargs: exec failed\n");
                exit(1);
            }
            wait(0);
            n = 0;
        }
        else if (n < 511) {
            line[n++] = c;
        }
        else {
            line[511] = 0;
        }
    }
    line[n] = 0;
    cmd[fixed] = line;
    cmd[fixed+1] = 0;
    int pid = fork();
    if (pid == 0) {
        exec(argv[1], cmd);
        fprintf(2, "xargs: exec failed\n");
        exit(1);
    }
    wait(0);
    n = 0;
    exit(0);
}