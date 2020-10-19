#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFSIZE 256
#define SIZE 8

int main(void) {
  char buf[BUFSIZE];
  int fildes[2];
  int status;

  if (pipe(fildes) < 0) {
    fprintf(stderr, "pipe error\n");
    exit(EXIT_FAILURE);
  }

  switch (fork()) {
    case -1:
      fprintf(stderr, "fork error\n");
      exit(EXIT_FAILURE);
    case 0:
      // child
      printf("child: buf points to %x\n", buf);
      close(fildes[1]);  // close write fd
      int n;
      void *ptr = buf;
      while ((n = read(fildes[0], ptr, SIZE)) > 0) {
        ptr += n;
      }
      printf("Message read: %s\n", buf);
      close(fildes[0]);
      exit(EXIT_SUCCESS);
    default:
      // parent
      printf("parent: buf points to %x\n", buf);
      close(fildes[0]);  // close read fd
      strcpy(buf, "We can write (almost) arbitrarily long messages here yayayayayayayayaya");
      if (write(fildes[1], buf, strlen(buf)) < 0) {
        fprintf(stderr, "pipe write error\n");
        exit(EXIT_FAILURE);
      }
      close(fildes[1]);
      wait(&status);
      exit(EXIT_SUCCESS);
  }
}
