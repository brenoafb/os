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
    char *argv[] = {"/usr/bin/ls", (char *) NULL};
    char *argvp[] = {"ls", (char *) NULL};
    execl("/usr/bin/ls", "/usr/bin/ls", (char *) NULL);
    // execlp("ls", "ls", (char *) NULL);
    // execle("/usr/bin/ls", "/usr/bin/ls", (char *) NULL, NULL);
    // execv(argv[0], argv);
    // execvp(argvp[0], argvp);

  } else {
    // parent
    wait(&status);
  }
}
