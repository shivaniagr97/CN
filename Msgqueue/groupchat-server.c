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

struct pidgroup{
  long pid;
  int grp;
}pidlist[100];
int n=0;

int search(long x){
  for(int i=0;i<n;i++){
    if(pidlist[i].pid==x) return i+1;
  }
  return 0;
}

void func(){
  msgctl(msgid,IPC_RMID,NULL);
  msgctl(msgid2,IPC_RMID,NULL);
  for(int i=0;i<n;i++) kill(pidlist[i].pid,SIGKILL);
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
    int ind = search(message_rcv.type);
    // printf("ind: %d",ind);
    if(!ind){
      pidlist[n].pid=message_rcv.type;
      pidlist[n++].grp=message_rcv.text[0]-48;
      continue;
    }
    struct message_buffer  message_sent = message_rcv;
    for(int i=0;i<n;i++){
      if(pidlist[i].pid==message_rcv.type) continue;
      if(pidlist[i].grp!=pidlist[ind-1].grp) continue;
      message_sent.type = pidlist[i].pid;
      msgsnd(msgid2,&message_sent,sizeof(message_sent),0);
    }
  }
}
