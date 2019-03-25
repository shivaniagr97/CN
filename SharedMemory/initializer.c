#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include<sys/types.h>
#include<stdlib.h>
#include<fcntl.h>

#define s1 "/semaphore1"
#define s2 "/semaphore2"

#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

int main(){
  sem_t *sem1 = sem_open(s1,O_CREAT | O_EXCL,SEM_PERMS,0);
  sem_t *sem2 = sem_open(s2,O_CREAT | O_EXCL,SEM_PERMS,1);
  if(sem1==SEM_FAILED||sem2==SEM_FAILED){
    perror("sem open failed");
    exit(EXIT_FAILURE);
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
  return 0;
}
