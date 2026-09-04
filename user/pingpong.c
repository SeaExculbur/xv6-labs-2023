 #include "kernel/types.h"
  #include "user/user.h"

  int main() {
      int p2c[2], c2p[2];
      pipe(p2c);
      pipe(c2p);

      int pid = fork();
      if(pid == 0){
          char *reply = "pong";
          char rec[8];
          int n = read(p2c[0], rec, 8);
          rec[n] = 0;
          printf("%d: received %s\n", getpid(), rec);
          write(c2p[1], reply, 4);
          exit(0);
      } else {
          char *msg = "ping";
          char rec[8];
          write(p2c[1], msg, 4);
          int n = read(c2p[0], rec, 8);
          rec[n] = 0;
          printf("%d: received %s\n", getpid(), rec);
          wait(0);
          exit(0);
      }
  }