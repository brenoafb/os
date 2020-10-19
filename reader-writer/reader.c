#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/shm.h>

#define KEY 0x1234
#define SIZE 512

typedef struct {
  long type;
  pid_t pid;
} pid_msg;

typedef struct {
  long type;
  char buf[SIZE];
} msg;

void ack_handler();
void msg_handler();
void int_handler();
void end_handler();

int pid_ack = 0;
int end = 0;
int exit_code = EXIT_SUCCESS;
int *msgsz;         // shared memory
int msgid, shmid;
pid_msg pid_send;
msg msg_rec;

int main(void) {
  printf("Reader: initializing\n");

  if ((msgid = msgget(KEY, 0)) < 0) {
    fprintf(stderr, "Reader: error obtaining msgid\n");
    exit(EXIT_FAILURE);
  }

  if ((shmid = shmget(KEY, sizeof(int), 0)) < 0) {
    fprintf(stderr, "Reader: error obtaining shmid\n");
    exit(EXIT_FAILURE);
  }

  if ((msgsz = shmat(shmid, NULL, 0)) < 0) {
    fprintf(stderr, "Reader: shared memory attach error\n");
    exit(EXIT_FAILURE);
  }

  printf("Reader: sending PID message (PID=%d)\n", getpid());
  pid_send.type = 1;
  pid_send.pid = getpid();

  signal(SIGUSR1, ack_handler);

  if (msgsnd(msgid, &pid_send, sizeof(pid_msg), 0) < 0) {
    fprintf(stderr, "Reader: error sending pid message\n");
    exit(EXIT_FAILURE);
  }

  printf("Reader: PID message sent, waiting for ACK\n");

  while(!pid_ack) {}

  printf("Reader: received ACK, waiting for message\n");

  signal(SIGUSR1, msg_handler);
  signal(SIGUSR2, end_handler);
  signal(SIGINT, int_handler);

  while (!end) {}

  printf("Reader: finished reception\n");

  if (end == 1) {
    exit(EXIT_SUCCESS);
  } else {
    exit(EXIT_FAILURE);
  }
}

void ack_handler() {
  pid_ack = 1;
}

void msg_handler() {
  if (*msgsz < 0) {
    printf("Reader: received termination\n");
    end = 1;
  } else {
    printf("*msgsz: %d\n", *msgsz);
    int nbytes = msgrcv(msgid, &msg_rec, *msgsz, 0, 0);
    printf("nbytes: %d\n", nbytes);
    if (nbytes < 0) {
      fprintf(stderr, "Reader: message receive error\n");
      end = 2;
    } else if (nbytes < 0) {
      end = 1;
    } else {
      printf("Reader: received message \"%s\"\n", msg_rec.buf);
    }
  }
}

void int_handler() {
  printf("Reader: SIGINT, finishing\n");
  end = 1;
}

void end_handler() {
  printf("Reader: SIGUSR2, finishing\n");
  end = 1;
}
