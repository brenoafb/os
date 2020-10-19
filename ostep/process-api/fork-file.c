
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  if (argc < 2) return 0;
  char *filename = argv[1];
  FILE *fp = fopen(filename, "w");

  pid_t pid;
  int status;

  if ((pid = fork()) < 0) {
    fprintf(stderr, "fork error\n");
    return 1;
  } else if (pid == 0) {
    // child
    fprintf(fp, "hello from child\n");
    fclose(fp);
  } else {
    // parent
    wait(&status);
    fprintf(fp, "hello from parent\n");
    fclose(fp);
  }
}
