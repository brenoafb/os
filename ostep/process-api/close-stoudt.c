#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  pid_t pid;
  int status;

  if ((pid = fork()) < 0) {
    fprintf(stderr, "fork error\n");
    return 1;
  } else if (pid == 0) {
    // child
    printf("Hello from child\n");
    close(STDOUT_FILENO);
    printf("Goodbye from child\n");
  } else {
    // parent
    printf("Hello from parent\n");
    wait(&status);
    printf("exit status: %d\n", WEXITSTATUS(status));
    printf("Goodbye from parent\n");
  }
}
