#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFSIZE 256

int main(void) {
  char buf[BUFSIZE];
  int status;
  pid_t pid;

  strcpy(buf, "DEFAULT MESSAGE");

  if ((pid = fork()) < 0) {
    fprintf(stderr, "fork error");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    // child
    strcpy(buf, "message from child");
    exit(EXIT_SUCCESS);
  } else {
    // parent
    wait(&status);
    printf("parent: buf contains %s\n", buf);
  }
}
