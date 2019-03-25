#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>

int main(){
  struct sockaddr_in serveraddr;
  int sfd = 0;
  if((sfd = socket(AF_INET,SOCK_DGRAM,0))==-1){
    perror("socket(): ");
    exit(0);
  }

  struct hostent *host;
  host = gethostbyname("127.0.0.1");
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(5000);
  serveraddr.sin_addr = *((struct in_addr *)host->h_addr);
  bzero((void *)&serveraddr.sin_zero,8);

  while(1){
    char buf[1024];
    printf("Enter message to send (q to quit): ");
    gets(buf);
    if(strcmp(buf,"q")==0){
      close(sfd);
      break;
    }
    sendto(sfd,buf,strlen(buf),0,(struct sockaddr *)&serveraddr,sizeof(serveraddr));

    char buffer_recv[1024]={0};
    int addrlen=0;
    recvfrom(sfd,buffer_recv,1024,0,(struct sockaddr *)&serveraddr,&addrlen);
    if(strcmp(buffer_recv,"q")==0){
      close(sfd);
      exit(0);
    }
    printf("Message received: %s\n",buffer_recv);
  }
}
