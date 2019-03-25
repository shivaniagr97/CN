#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <fcntl.h>

#define fifo "wellknown"
int fd;

void sigint(int sig){
  close(fd);
  exit(0);
}

int main(){
  mkfifo(fifo,0666);
  fd = open(fifo,O_RDWR);
  signal(SIGINT,sigint);

  while(1){
    char buf[100]="";
    read(fd,buf,100);
    printf(buf);
  }
}
