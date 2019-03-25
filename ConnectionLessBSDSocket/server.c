#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>

int sfd = 0;

void sigint(int sig){
  close(sfd);
  exit(0);
}

int main(){

  signal(SIGINT,sigint);
  struct sockaddr_in serveraddr;
  if((sfd = socket(AF_INET,SOCK_DGRAM,0))==-1){
    perror("socket(): ");
    exit(0);
  }

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(5000);
  serveraddr.sin_addr.s_addr = INADDR_ANY;
  bzero((void *)&serveraddr.sin_zero,8);

  if(bind(sfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))==-1){
    perror("bind(): ");
    exit(0);
  }

  while(1){

    char buf[1024]={0};
    int len = 0;
    struct sockaddr_in clientaddr;
    int addrlen = sizeof(struct sockaddr_in);
    recvfrom(sfd,(void *)buf,1024,0,(struct sockaddr *)&clientaddr,&addrlen);
    printf("Message received from %s %d: %s\n",inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port),buf);

    char buffer_send[1024]={0};
    printf("Enter message to send (q to quit): ");
    gets(buffer_send);
    sendto(sfd,buffer_send,strlen(buffer_send),0,(struct sockaddr *)&clientaddr,sizeof(clientaddr));
  }

}
