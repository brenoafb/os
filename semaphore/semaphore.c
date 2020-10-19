#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define SEMKEY 0x1234
#define SHMKEY 0x1235
#define SIZE 32
#define ACCESS 0666

// initializes semaphore with value 1
// returns the semaphore's id
int init_sem();

int deinit_sem(int semid);

int read_sem(int semid);

// decrement semaphore by 1
// blocks if semaphore value was <= 0
int p_sem(int semid);

// increments semaphore by 1
int v_sem(int semid);

void sig_handler(int signum);

int main(void) {
  pid_t pid;
  int semid, shmid, status, sig;
  char *shm;
  sigset_t sigset;

  if ((semid = init_sem()) < 0) {
    fprintf(stderr, "semaphore error: %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  if ((shmid = shmget(SHMKEY, SIZE+1, IPC_CREAT | ACCESS)) < 0) {
    fprintf(stderr, "shared memory init error: %s", strerror(errno));
    deinit_sem(semid);
    exit(EXIT_FAILURE);
  }

  if ((pid = fork()) < 0) {
    fprintf(stderr, "fork error: %s", strerror(errno));
    deinit_sem(semid);
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    // child
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
    printf("Child: attaching to shm\n");
    if ((shm = shmat(shmid, NULL, 0)) < 0) {
      fprintf(stderr, "shared memory attach error: %s", strerror(errno));
      exit(EXIT_FAILURE);
    }

    printf("Child: waiting for SIGUSR1\n");
    sigwait(&sigset, &sig);
    printf("Child: SIGUSR1 received\n");
    while (shm[0] < SIZE) {
      usleep(10);
      p_sem(semid);
      shm[(int) ++shm[0]] = 'C';
      v_sem(semid);
    }

    printf("Child: child exiting\n");
    exit(EXIT_SUCCESS);

  } else {
    if ((shm = shmat(shmid, NULL, 0)) < 0) {
      fprintf(stderr, "shared memory attach error: %s", strerror(errno));
      shmctl(shmid, IPC_RMID, 0);
      deinit_sem(semid);
      exit(EXIT_FAILURE);
    }

    sleep(1);
    printf("Parent: setting initial memory values\n");
    memset(shm, 0, SIZE+1);
    printf("Parent: sending SIGUSR1\n");
    kill(pid, SIGUSR1);
    printf("Parent: SIGUSR1 sent\n");

    while (shm[0] < SIZE) {
      usleep(10);
      p_sem(semid);
      shm[(int) ++shm[0]] = 'P';
      v_sem(semid);
    }
  }

  wait(&status);

  printf("Final state of shared memory:\n");
  printf("%d\n", (int) shm[0]);
  for (int i = 0; i < SIZE; i++){
    printf("%c", shm[i]);
  }
  printf("\n");

  shmctl(shmid, IPC_RMID, 0);
  deinit_sem(semid);
}

int init_sem() {
  int semid = semget(SEMKEY, 1, IPC_CREAT | ACCESS);

  if (semid < 0) {
    return semid;
  }

  v_sem(semid);

  return semid;
}

int deinit_sem(int semid) {
  return semctl(semid, 0, IPC_RMID);
}

int read_sem(int semid) {
  return semctl(semid, 0, GETVAL);
}

int p_sem(int semid) {
  struct sembuf sop;
  sop.sem_num = 0;
  sop.sem_op = -1;
  sop.sem_flg = 0;
  return semop(semid, &sop, 1);
}

int v_sem(int semid) {
  struct sembuf sop;
  sop.sem_num = 0;
  sop.sem_op = 1;
  sop.sem_flg = 0;
  return semop(semid, &sop, 1);
}

void sig_handler(int signum) {
  printf("Caught singal: %d\n", signum);
}
