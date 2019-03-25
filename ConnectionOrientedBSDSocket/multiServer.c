#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/select.h>
#include <signal.h>
#include <string.h>

#define N 3
int sfd[3] = {0};

void sigint(int sig){
  for(int i=0;i<N;i++){
    close(sfd[i]);
  }
  exit(0);
}

int findMax(int sfd[],int n){
  int max = -1;
  for(int i=0;i<n;i++){
    if(max<sfd[i]) max=sfd[i];
  }
  return max;
}

int main(){

  struct sockaddr_in serveraddr[3],clientaddr;

  for(int i=0;i<N;i++){
    if((sfd[i] = socket(AF_INET,SOCK_STREAM,0))==-1){
      perror("socket(): ");
      exit(1);
    }
  }

  for(int i=0;i<N;i++){
    serveraddr[i].sin_family = AF_INET;
    serveraddr[i].sin_port = htons(5000+i);
    serveraddr[i].sin_addr.s_addr = INADDR_ANY;
    bzero((void *)&serveraddr[i].sin_zero,8);
  }


  for(int i=0;i<N;i++){
    int true = 1;
    if (setsockopt(sfd[i],SOL_SOCKET,SO_REUSEADDR,&true,sizeof(int)) == -1) {
              perror("Setsockopt");
              exit(1);
    }
    if(bind(sfd[i],(struct sockaddr *)&serveraddr[i],sizeof(serveraddr[i]))==-1){
      perror("bind(): ");
      exit(1);
    }
  }

  for(int i=0;i<N;i++){
    if(listen(sfd[i],5)==-1){
      perror("listen(): ");
      exit(1);
    }
  }

  fd_set rfds;
  struct timeval tv;
  int retval=0;
  tv.tv_sec = 5;
  tv.tv_usec = 0;
  while(1){
    FD_ZERO(&rfds);
    for(int i=0;i<N;i++){
      FD_SET(sfd[i],&rfds);
    }
    int max = findMax(sfd,N);
    retval = select(max+1,&rfds,NULL,NULL,&tv);
    if(retval>0){
      for(int i=0;i<N;i++){
        if(FD_ISSET(sfd[i],&rfds)){
          int len=sizeof(struct sockaddr_in);
          int nsfd = accept(sfd[i],(struct sockaddr *)&clientaddr,&len);
          // printf("%d",nsfd);
          int pid = fork();
          if(pid<0){
            perror("fork(): ");
            exit(1);
          }
          if(pid>0){
            close(nsfd);
          }
          else{
            for(int j=0;j<N;j++){
              close(sfd[j]);
            }
            // printf("%s\n", "here");
            fflush(stdout);

            char temp[12];
            sprintf(temp,"%d",nsfd);
            // dup2(nsfd,0);
            // dup2(nsfd,1);

            char *arg[]={"s",temp,NULL};
            execv(arg[0],arg);
          }
        }
      }
    }
  }
}
