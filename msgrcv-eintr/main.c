#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/msg.h>

#define KEY 0x1234
#define ACCESS 0777
#define INTERVAL 2
#define SIZE 100

typedef struct {
  long type;
  char msg[SIZE];
} msg;

void handler();

int main() {
  int msgid;
  msg msgrec;
  printf("Creating message queue\n");

  if ((msgid = msgget(KEY, IPC_CREAT | ACCESS)) < 0) {
    fprintf(stderr, "Error creating queue\n");
    exit(EXIT_FAILURE);
  }

  printf("Message queue created with key 0x%x, id %d\n", KEY, msgid);
  printf("Waiting for message...\n");

  signal(SIGALRM, handler);
  alarm(INTERVAL);
  if (msgrcv(msgid, &msgrec, sizeof(msgrec), 0, 0) == -1) {
    printf("Time limit exceeded\n");
  } else {
    printf("Received message: %s\n", msgrec.msg);
  }

  msgctl(msgid, IPC_RMID, NULL);

  exit(EXIT_SUCCESS);
}

void handler() {
  //
}
