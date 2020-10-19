#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

void sigusr1_handler(int s);
void sigusr2_handler(int s);

int main(void) {
  pid_t pid = getpid();

  signal(SIGUSR1, sigusr1_handler);
  signal(SIGUSR2, sigusr2_handler);

  kill(pid, SIGUSR1);
  kill(pid, SIGUSR2);

}

void sigusr1_handler(int s) {
  printf("Received SIGUSR1, s: %d\n", s);
}

void sigusr2_handler(int s) {
  printf("Received SIGUSR2, s: %d\n", s);
}
