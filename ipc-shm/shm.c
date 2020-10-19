#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SIZE 32
#define KEY 0x1234

int main(void) {
  pid_t pid;
  int shm_id;
  char *shm_ptr;
  char buf[SIZE];
  int status;

  strcpy(buf, "GARBAGE");

  if ((shm_id = shmget(KEY, SIZE, IPC_CREAT|0x1ff)) < 0) {
    fprintf(stderr, "Error getting shared memory segment");
    exit(EXIT_FAILURE);
  }

  if ((pid = fork()) < 0) {
    fprintf(stderr, "Fork error");
    shmctl(shm_id, IPC_RMID, NULL);
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    // child
    if ((shm_ptr = shmat(shm_id, NULL, 0)) < 0) {
      fprintf(stderr, "attach error");
      exit(EXIT_FAILURE);
    }

    strcpy(shm_ptr, "Hello from shm");
    exit(EXIT_SUCCESS);
  } else {
    // parent
    wait(&status);
    if (WEXITSTATUS(status) == EXIT_FAILURE) {
      shmctl(shm_id, IPC_RMID, NULL);
      exit(EXIT_FAILURE);
    } else if (WEXITSTATUS(status) == EXIT_SUCCESS) {
      if ((shm_ptr = shmat(shm_id, NULL, SHM_RDONLY)) < 0) {
        fprintf(stderr, "attach error");
        exit(EXIT_FAILURE);
      }
      strcpy(buf, shm_ptr);
      printf("Received message from shared memory: %s\n", buf);
    }
  }

  shmctl(shm_id, IPC_RMID, NULL);

  exit(EXIT_SUCCESS);
}
