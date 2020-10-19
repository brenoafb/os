#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFSIZE 256

void child1() {

}

void child2() {

}

int main(int argc, char *argv[]) {
  pid_t pid1, pid2;
  int status;
  int fildes[2];
  int nbytes;
  char buf[BUFSIZE];
  char *msg = "Hello from child 1!";
  int len = strlen(msg) + 1;

  status = pipe(fildes);
  if (status == -1) {
    // pipe error
  }

  if ((pid1 = fork()) < 0) {
    fprintf(stderr, "fork error\n");
    return 1;
  } else if (pid1 == 0) {
    // child 1 writes to pipe
    close(fildes[0]);  // close read handle
    nbytes = write(fildes[1], msg, strlen(msg) + 1);
    printf("Child 1 wrote %d bytes to pipe\n", nbytes);
    close(fildes[1]);
    exit(EXIT_SUCCESS);
  } else {
    // parent
    if ((pid2 = fork()) < 0) {
      fprintf(stderr, "fork error\n");
      return 2;
    } else if (pid2 == 0) {
      // child 2 reads from pipe
      close(fildes[1]);  // close write handle

      int i = 0;
      nbytes = 0;
      do {
        nbytes += read(fildes[0], buf, 1);
        printf("read %c (%d) at %d\n", buf[i], buf[i], i);
      } while (buf[i++] != 0);
      printf("Child 2 read: %s\n", buf);

      close(fildes[0]);
      exit(EXIT_SUCCESS);
    } else {
      // parent
      // TODO: handle status
      wait(&status);
      wait(&status);
    }
  }
}
