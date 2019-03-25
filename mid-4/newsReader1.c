#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>

#define s1 "/semaphore1"
#define s2 "/semaphore2"
#define x "xy"
#define fifo "wellknown"
struct messagequeuestruct{
  long type;
  char text[100];
}messageBuf;

int msqid,fd;

void sigint(int sig){
  close(fd);
  exit(0);
}

int main(){

  int key1 = ftok(x,66);
  int shmid = shmget(key1,1024,IPC_CREAT|0666);
  char * shm = (char *)shmat(shmid,NULL,0);

  sem_t *sem1 = sem_open(s1,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
  sem_t *sem2 = sem_open(s2,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);

  int key2 = ftok("message",65);
  printf("%d\n",key2 );
  msqid = msgget(key2,IPC_CREAT|0666);

  mkfifo(fifo,0666);
  fd = open(fifo,O_RDWR);

  signal(SIGINT,sigint);

  while(1){
    sem_wait(sem1);

    // printf("here");
    msgrcv(msqid,&messageBuf,sizeof(messageBuf),0,0);
    // printf("here1");
    printf(messageBuf.text);

    if(isdigit(messageBuf.text[0])){
      int port = atoi(messageBuf.text);
      int sfd=0;
      struct sockaddr_in serveraddr,clientaddr;

      struct hostent *host;
      host = gethostbyname("127.0.0.1");
      serveraddr.sin_family = AF_INET;
      serveraddr.sin_port = htons(port);
      serveraddr.sin_addr = *((struct in_addr *)host->h_addr);
      bzero((void *)&(serveraddr.sin_zero),8);

      if((sfd = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("socket(): ");
        exit(0);
      }

      if(connect(sfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))==-1){
        perror("connect(): ");
        exit(1);
      }

      while(1){
        char buf[100]="";
        if(recv(sfd,buf,100,0)==-1){
          perror("recv(): ");
          exit(1);
        }
        if(buf[0]=='q'){
          printf("%s\n", "quitting........");
          close(sfd);
          break;
        }
        write(fd,buf,strlen(buf));
      }
    }

    else write(fd,messageBuf.text,strlen(messageBuf.text));

    sem_post(sem2);
  }

}
