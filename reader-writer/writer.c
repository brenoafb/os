#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/shm.h>

#define KEY 0x1234
#define SIZE 512
#define ACCESS 0777

typedef struct {
  long type;
  pid_t pid;
} pid_msg;

typedef struct {
  long type;
  char buf[SIZE];
} msg;

int send_message(char *message);
void int_handler();

int end = 0;
int *msgsz;         // shared memory
int msgid = 0;
int shmid = 0;
pid_msg pid_rec;
pid_t reader_pid = 0;
msg msg_send;

int main(void) {
  char *msg_str = "Hello from writer!";
  signal(SIGINT, int_handler);

  printf("Writer: initializing message queue\n");

  if ((msgid = msgget(KEY, IPC_CREAT | ACCESS)) < 0) {
    fprintf(stderr, "Writer: error creating message queue\n");
    exit(EXIT_FAILURE);
  }

  printf("Writer: initializing shared memory\n");

  if ((shmid = shmget(KEY, sizeof(int), IPC_CREAT | ACCESS)) < 0) {
    fprintf(stderr, "Writer: error creating shared memory segment\n");
    msgctl(msgid, IPC_RMID, NULL);
    exit(EXIT_FAILURE);
  }

  printf("Writer: attaching shared memory\n");

  if ((msgsz = shmat(shmid, NULL, 0)) < 0) {
    fprintf(stderr, "Writer: shared memory attach error\n");
    exit(EXIT_FAILURE);
  }

  printf("Writer: waiting for PID message\n");

  if (msgrcv(msgid, &pid_rec, sizeof(pid_msg), 0, 0) < 0) {
    fprintf(stderr, "Writer: shared memory attach error\n");
    msgctl(msgid, IPC_RMID, NULL);
    shmctl(shmid, IPC_RMID, NULL);
    exit(EXIT_FAILURE);
  }

  reader_pid = pid_rec.pid;
  printf("Writer: received PID %d, sending ACK\n", reader_pid);
  kill(reader_pid, SIGUSR1);
  printf("Writer: sending message\n");

  if (send_message(msg_str) < 0) {
    fprintf(stderr, "Writer: message send error\n");
    msgctl(msgid, IPC_RMID, NULL);
    shmctl(shmid, IPC_RMID, NULL);
    kill(reader_pid, SIGUSR2);
    exit(EXIT_FAILURE);
  }

  printf("Writer: message sent, sending termination\n");
  sleep(1);

  if (send_message(NULL) != 0) {
    fprintf(stderr, "Writer: termination message error\n");
    msgctl(msgid, IPC_RMID, NULL);
    shmctl(shmid, IPC_RMID, NULL);
    kill(reader_pid, SIGUSR2);
    exit(EXIT_FAILURE);
  }

  printf("Writer: termination sent, finalizing\n");

  msgctl(msgid, IPC_RMID, NULL);
  shmctl(shmid, IPC_RMID, NULL);

  exit(EXIT_SUCCESS);
}

int send_message(char *message) {
  if (!message) {
    kill(reader_pid, SIGUSR1);
    *msgsz = -1;
    return 0;
  }
  *msgsz = strlen(message) + 1;
  printf("*msgsz: %d\n", *msgsz);
  msg_send.type = 1;
  strncpy(msg_send.buf, message, *msgsz);
  msgsnd(msgid, &msg_send, *msgsz, 0);
  kill(reader_pid, SIGUSR1);
  return 0;
}

void int_handler() {
  printf("Writer: received SIGINT\n");
  if (msgid) msgctl(msgid, IPC_RMID, NULL);
  if (shmid) shmctl(shmid, IPC_RMID, NULL);
  if (reader_pid) kill(reader_pid, SIGUSR2);
  exit(EXIT_SUCCESS);
}
