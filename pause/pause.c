#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler();

int main() {
  signal(SIGALRM, handler);
  alarm(1);

  printf("Pausing thread\n");
  pause();
  printf("Resuming thread\n");
}

void handler() {
  printf("Hello from signal handler\n");
}
