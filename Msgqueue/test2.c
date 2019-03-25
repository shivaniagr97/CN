#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

int msgid;


struct message_buffer{
  long type;
  char text[512];
} message_sent,message_rcv;

void sigint(){
  struct message_buffer message_sent;
  scanf("%s",message_sent.text);
  message_sent.type = 2;
  msgsnd(msgid, &message_sent, sizeof(message_sent), 0);
  return;
}


int main()
{
  signal(SIGINT,sigint);
  int key = ftok("prog",0);
  msgid = msgget(key, 0666|IPC_CREAT);
  while(1){
    if(msgrcv(msgid, &message_rcv, sizeof(message_rcv), 3, 0)!=-1)
    printf("P2: %s\n", message_rcv.text);
  }

    msgctl(msgid, IPC_RMID, NULL);
}
