#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
  int x = 10;

  pid_t pid;
  int status;

  if ((pid = fork()) < 0) {
    fprintf(stderr, "Fork error\n");
    return 1;
  } else if (pid == 0) {
    // child
    printf("Child (%d): x=%d\n", getpid(), x);
    x = 0;
    return 0;
  } else {
    // parent
    printf("Parent (before wait) (%d): x=%d\n", getpid(), x);
    wait(&status);
    printf("Parent (after wait) (%d): x=%d\n", getpid(), x);
  }
}
