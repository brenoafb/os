#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define SIZE 16
#define KEY 0x1234

typedef struct msg {
  long l;
  char msg[SIZE];
} msg;

int main(void) {
  pid_t pid;
  int status;
  int queue_id;

  msg msg_send, msg_rec;

  if ((queue_id = msgget(KEY, IPC_CREAT|0777)) < 0) {
    fprintf(stderr, "queue creation error");
    exit(EXIT_FAILURE);
  }

  printf("Created queue with id %d\n", queue_id);

  if ((pid = fork()) < 0) {
    fprintf(stderr, "fork error");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    // child
    printf("Child sending message (queue id: %d)\n", queue_id);
    msg_send.l = getpid();
    strcpy(msg_send.msg, "Hello!");
    msgsnd(queue_id, &msg_send, sizeof(msg_send), 0);
    printf("Child done\n");
    exit(EXIT_SUCCESS);
  }

  printf("Parent waiting to receive message (queue id: %d)\n", queue_id);
  msgrcv(queue_id, &msg_rec, sizeof(msg_rec), 0, 0);
  printf("Received message: %ld %s\n", msg_rec.l, msg_rec.msg);

  wait(&status);

  msgctl(queue_id, IPC_RMID, NULL);

  exit(EXIT_SUCCESS);
}
