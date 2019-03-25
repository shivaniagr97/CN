#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>

struct message_buffer{
  long type;
  char text[512];
} message_rcv;

int msgid1;

void sigint(){
  struct message_buffer message_sent;
  scanf("%s",message_sent.text);
  message_sent.type = getpid();
  msgsnd(msgid1, &message_sent, sizeof(message_sent), 0);
  return;
}

int main()
{
  signal(SIGINT,sigint);
  int key = ftok("/home/shivani/CN/Msgqueue/prog",0);
  msgid1 = msgget(key, 0666|IPC_CREAT);
  int key2 = ftok("/home/shivani/CN/Msgqueue/stoc",1);
  int msgid2 = msgget(key2, 0666|IPC_CREAT);
  printf("%d %d",msgid1,msgid2);
  while(1){
    msgrcv(msgid2, &message_rcv, sizeof(message_rcv), getpid() , 0);
    printf("%s\n", message_rcv.text);
  }
}
