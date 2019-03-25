#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>


#define s1 "/semaphore1"
#define s2 "/semaphore2"
#define x "x"
#define y "y"

void sigint(int sig){
  sem_unlink(s1);
  sem_unlink(s2);
  exit(0);
}

int main(){

  signal(SIGINT,sigint);

  sem_t *sem1 = sem_open(s1,O_RDWR);
  sem_t *sem2 = sem_open(s2,O_RDWR);
  // sem_unlink(s1);
  // sem_unlink(s2);

  if(sem1==SEM_FAILED||sem2==SEM_FAILED){
    perror("sem open failed");
    exit(EXIT_FAILURE);
  }

  int key1 = ftok(x,65);
  int key2 = ftok(y,65);

  // printf("%d %d\n",key1,key2);
  int shmid1 = shmget(key1,1024,0666|IPC_CREAT);
  int shmid2 = shmget(key2,1024,0666|IPC_CREAT);

  char *strx = (char *)shmat(shmid1, NULL, 0);
  char *stry = (char *)shmat(shmid2, NULL, 0);

  strcpy(strx,"0");
  strcpy(stry,"0");

  while(1){
    sem_wait(sem1);
    int dx = atoi(strx);
    int dy = dx+1;
    sprintf(stry,"%d",dy);
    printf("x: %d\n",dx);
    printf("y: %d\n",dy);
    fflush(stdout);
    sleep(2);
    sem_post(sem2);
  }

}
