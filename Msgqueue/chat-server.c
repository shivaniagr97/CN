#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

int msgid,msgid2;


struct message_buffer{
  long type;
  char text[512];
} ;

long pidlist[100];
int n=0;

int search(long x){
  for(int i=0;i<n;i++){
    if(pidlist[i]==x) return 1;
  }
  return 0;
}

void func(){
  msgctl(msgid,IPC_RMID,NULL);
  msgctl(msgid2,IPC_RMID,NULL);
  for(int i=0;i<n;i++) kill(pidlist[i],SIGKILL);
  exit(0);
}

int main()
{
  signal(SIGINT,func);
  int key = ftok("/home/shivani/CN/Msgqueue/prog",0);
  msgid = msgget(key, 0666|IPC_CREAT);
  int key2 = ftok("/home/shivani/CN/Msgqueue/stoc",1);
  msgid2 = msgget(key2, 0666|IPC_CREAT);
  while(1){
    struct message_buffer message_rcv;
    if(msgrcv(msgid, &message_rcv, sizeof(message_rcv), 0, 0)!=-1)
    printf("%s\n", message_rcv.text);
    if(!search(message_rcv.type)) pidlist[n++]=message_rcv.type;

    struct message_buffer  message_sent = message_rcv;
    for(int i=0;i<n;i++){
      if(pidlist[i]==message_rcv.type) continue;
      message_sent.type = pidlist[i];
      msgsnd(msgid2,&message_sent,sizeof(message_sent),0);
    }
  }
}
