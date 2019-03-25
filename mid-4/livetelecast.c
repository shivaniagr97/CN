#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

int sfd,nsfd;

void sigint(int sig){
  close(sfd);
  close(nsfd);
  exit(1);
}

int main(){
  sfd = socket(AF_INET,SOCK_STREAM,0);
  if(sfd == -1){
    perror("socket(): ");
    exit(1);
  }
  struct sockaddr_in serveraddr,clientaddr;
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(5000);
  serveraddr.sin_addr.s_addr = INADDR_ANY;
  bzero((void *)serveraddr.sin_zero,8);

  int opt=1;
  setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,sizeof(opt));

  if(bind(sfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))==-1){
    perror("bind(): ");
    exit(1);
  }

  if(listen(sfd,5)==-1){
    perror("listen(): ");
    exit(1);
  }
  while(1){
    int len = sizeof(struct sockaddr_in);
    nsfd = accept(sfd,(struct sockaddr *)&clientaddr,&len);

    while(1){
      char buf[100]="";
      read(STDIN_FILENO,buf,100);
      if(buf[0]=='q'){
        send(nsfd,buf,strlen(buf),0);
        close(nsfd);
        break;
      }
      if(send(nsfd,buf,strlen(buf),0)==-1){
        perror("send(): ");
        exit(1);
      }
    }
  }

}
