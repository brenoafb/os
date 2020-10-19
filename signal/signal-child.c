#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>

void sigusr1_handler();
void sigusr2_handler();

int main(void) {
  pid_t pid;
  int status;

  if ((pid = fork()) < 0) {
    fprintf(stderr, "fork error\n");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    signal(SIGUSR1, sigusr1_handler);
    signal(SIGUSR2, sigusr2_handler);
    exit(EXIT_SUCCESS);
  } else {
    kill(pid, SIGUSR1);
  }

  wait(&status);

  exit(EXIT_SUCCESS);
}


void sigusr1_handler() {
  printf("Received SIGUSR1\n");
}

void sigusr2_handler() {
  printf("Received SIGUSR2\n");
}
