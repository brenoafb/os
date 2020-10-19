#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int s);

int flag;

#define N 1000000

int main(void) {
  flag = 1;
  signal(SIGALRM, handler);
  alarm(1);

  int x = 0;
  while(flag) {
    x += 1;
  }

  printf("x=%d\n", x);
}

void handler(int s) {
  printf("Received a sigalarm!\n");
  flag = 0;
}
