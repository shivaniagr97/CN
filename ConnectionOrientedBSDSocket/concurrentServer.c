#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>
int sfd = 0;

void sigint(int sig){
  close(sfd);
  exit(0);
}

int main(){

  signal(SIGINT,sigint);

  struct sockaddr_in serveraddr,clientaddr;

  if((sfd=socket(AF_INET,SOCK_STREAM,0))==-1){
    perror("socket(): ");
    exit(1);
  }

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(5000);
  serveraddr.sin_addr.s_addr = INADDR_ANY;
  bzero((void *)&(serveraddr.sin_zero),8);

  if(bind(sfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))==-1){
    perror("bind(): ");
    exit(1);
  }

  if(listen(sfd,5)==-1){
    perror("listen(): ");
    exit(1);
  }

  while(1){
    int addrlen = sizeof(struct sockaddr_in);
    int nsfd = accept(sfd,(struct sockaddr *)&clientaddr,&addrlen);
    if(nsfd==-1){
      perror("accept(): ");
      exit(1);
    }
    int pid = fork();

    if(pid<0){
      perror("fork(): ");
      exit(0);
    }
    if(pid==0){
      close(sfd);
      while(1){
        char buffer_recv[1024];
        int bytes_recv=0;
        if((bytes_recv=recv(nsfd,buffer_recv,1024,0))==-1){
          perror("recv(): ");
          exit(1);
        }
        buffer_recv[bytes_recv]='\0';
        if(strcmp(buffer_recv,"q")==0){
          close(nsfd);
          exit(0);
        }
        printf("Received message: %s\n",buffer_recv);

        char buffer_send[1024];
        printf("Enter message to send /press q to quit: ");
        gets(buffer_send);
        send(nsfd,buffer_send,strlen(buffer_send),0);
        if(strcmp(buffer_send,"q")==0){
          close(nsfd);
          exit(0);
        }
      }
    }
    else{
      close(nsfd);
      continue;
    }
  }
}
