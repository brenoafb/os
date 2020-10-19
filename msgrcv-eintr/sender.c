#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/msg.h>

#define KEY 0x1234
#define SIZE 100

typedef struct {
  long type;
  char msg[SIZE];
} msg;

int main(int argc, char *argv[]) {
  int msgid;
  msg msg;

  if ((msgid = msgget(KEY, 0)) < 0) {
    fprintf(stderr, "Error getting queue id\n");
    exit(EXIT_FAILURE);
  }

  printf("Obtained msgid with key 0x%x, id %d\n", KEY, msgid);
  printf("Sending message...\n");

  msg.type = 1;
  strncpy(msg.msg, "Hello there!", SIZE);

  if (msgsnd(msgid, &msg, sizeof(msg), 0) < 0) {
    printf("Error sending message\n");
  } else {
    printf("Message sent: %s\n", msg.msg);
  }

  msgctl(msgid, IPC_RMID, NULL);

  exit(EXIT_SUCCESS);
}

void handler() {
  //
}
