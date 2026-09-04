#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int p1[2];
    int p2[2];
    pipe(p1);
    pipe(p2);
    int pid = fork();
    if (pid == 0) {
        char *data = "pong";
        char rec[8];
        int n = read(p1[0], &rec, 8);
        rec[n] = 0;
        printf("%d: recevied %s\n", getpid(), rec);
        write(p2[1], data, 4);
        exit(0);
    }
    else {
        char *data = "ping";
        char rec[8];
        write(p1[1], data, 8);
        wait(0);
        int n = read(p2[0], &rec, 8);
        rec[n] = 0;
        printf("%d: recevied %s\n", getpid(), rec);
        exit(0);
    }
}