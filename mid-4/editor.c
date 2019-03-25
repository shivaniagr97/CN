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

FILE* r[3];
int reporterfd[3];

#define s1 "/semaphore1"
#define s2 "/semaphore2"
#define x "xy"

#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

struct messagequeuestruct{
  long type;
  char text[100];
}messageBuf;

int msqid;
int newsshmid;

void initSem(){
  sem_t *sem1 = sem_open(s1,O_CREAT | O_EXCL,SEM_PERMS,0);
  sem_t *sem2 = sem_open(s2,O_CREAT | O_EXCL,SEM_PERMS,1);
  if(sem1==SEM_FAILED||sem2==SEM_FAILED){
    sem1 = sem_open(s1,SEM_PERMS);
    sem2 = sem_open(s2,SEM_PERMS);
  }

  if(sem_close(sem1)<0)
  {
    perror("sem_closed error");
    exit(0);
  }
  if(sem_close(sem2)<0)
  {
    perror("sem_closed error");
    exit(0);
  }
}

//popen all reporters
void initReporters(){
  fflush(stdin);
  fflush(stdout);
  r[0] = popen("./r1","r");
  r[1] = popen("./r2","r");
  r[2] = popen("./r3","r");
  if(!r[1] || !r[2] || !r[0]){
    perror("popen(): ");
    exit(1);
  }
  reporterfd[0] = fileno(r[0]);
  reporterfd[1] = fileno(r[1]);
  reporterfd[2] = fileno(r[2]);
}


void initMsgQueue(){
  int key = ftok("message",65);
  msqid = msgget(key,IPC_CREAT|0666);
}

void initshm(){
  int key1 = ftok(x,66);

  int shmid1 = shmget(key1,1024,0666|IPC_CREAT);

  char *strx = (char *)shmat(shmid1, NULL, 0);

  strcpy(strx,"0");
}

//close all popen fds on exit
void sigint(int sig){
  for(int i=0;i<3;i++){
    pclose(r[i]);
  }
  msgctl(msqid,IPC_RMID,NULL);
  sem_unlink(s1);
  sem_unlink(s2);
  shmctl(newsshmid,IPC_RMID,NULL);
  exit(0);
}

//find max sfd to pass in select
int findMax(int sfd[],int n){
  int max = -1;
  for(int i=0;i<n;i++){
    if(max<sfd[i]) max=sfd[i];
  }
  return max;
}

int main(){

  initSem();
  initshm();
  //popen all reporters
  initReporters();

  initMsgQueue();

  signal(SIGINT, sigint);

  fd_set rfds;
  struct timeval tv;
  tv.tv_sec = 15;
  tv.tv_usec = 0;

  while(1){
    FD_ZERO(&rfds);
    for(int i=0;i<3;i++){
      FD_SET(reporterfd[i],&rfds);
    }
    int max = findMax(reporterfd,3);
    //check for news from reporters
    int retval = select(max+1,&rfds,NULL,NULL,&tv);
    if(retval==-1){
      perror("select(): ");
      exit(1);
    }
    else{
      for(int i=0;i<3;i++){
        if(FD_ISSET(reporterfd[i],&rfds)){
          char buf[120]="";
          read(reporterfd[i],buf,120);
          write(STDOUT_FILENO, buf, 100);
          fflush(stdout);

          //if news has prefix /d then pass to document writer
          if(buf[0]=='/' && buf[1]=='d'){
            buf[0] = ' ';
            buf[1] = ' ';
            int pp[2];
            if(pipe(pp)==-1){
              perror("pipe(): ");
              exit(1);
            }
            int pid = fork();
            if(pid==0){
              close(pp[1]);
              dup2(pp[0],0);
              char * args[]={"d",NULL};
              execv(args[0],args);
            }
            else{
              close(pp[0]);
              write(pp[1],buf,strlen(buf));
              close(pp[1]);
            }
          }
          //if prefix isn't /d send msg to news reader
          else{
            printf("send to newsreader\n");
            messageBuf.type = getpid();
            strcpy(messageBuf.text,buf);
            msgsnd(msqid,&messageBuf,sizeof(messageBuf),0);
          }
        }
      }
    }
  }
}
