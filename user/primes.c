#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void sieve(int left_fd) {
    int p;
    int n = read(left_fd, &p, sizeof(int));
    if (n <= 0) return;
    printf("prime %d\n", p);

    int right[2];
    pipe(right);
    int pid = fork();
    if (pid == 0) {
        close(right[1]);
        close(left_fd);
        sieve(right[0]);
        exit(0);
    }
    else {
        close(right[0]);
        int res;
        while (read(left_fd, &res ,sizeof(int))) {
            if (res % p != 0) {
                write(right[1] ,&res ,sizeof(int));
            }
        }
        close(right[1]);
        wait(0);
        exit(0);
    }
}

int main() {
    int p[2];
    pipe(p);
    for (int i = 2; i <= 35; i++) {
        write(p[1] ,&i ,sizeof(int));
    }
    close(p[1]);
    sieve(p[0]);
    exit(0);
}