#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
int main(){

  int sfd=0;
  struct sockaddr_in serveraddr,clientaddr;
  char protocol[20];
  printf("%s\n", "Enter protocol: ");
  scanf("%s", protocol);

  int port;
  printf("%s\n", "Enter port: ");
  scanf("%d", &port);

  printf("%s %d\n",protocol,port );
  struct hostent *host;
  host = gethostbyname("127.0.0.1");
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(port);
  serveraddr.sin_addr = *((struct in_addr *)host->h_addr);
  bzero((void *)&(serveraddr.sin_zero),8);

 if(strcmp(protocol,"tcp")==0){

   printf("you selected tcp\n");
   if((sfd = socket(AF_INET,SOCK_STREAM,0))==-1){
     perror("socket(): ");
     exit(0);
   }

   if(connect(sfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))==-1){
     perror("connect(): ");
     exit(1);
   }

   while(1){
     char buffer_send[1024];
     printf("Enter message to send /press q to quit: ");
     fflush(stdout);
     scanf("%s",buffer_send);
     send(sfd,buffer_send,strlen(buffer_send),0);
     if(strcmp(buffer_send,"q")==0){
       close(sfd);
       break;
     }

     char buffer_recv[1024];
     int bytes_recv=0;
     if((bytes_recv=recv(sfd,buffer_recv,1024,0))==-1){
       perror("recv(): ");
       exit(1);
     }
     buffer_recv[bytes_recv]='\0';
     if(strcmp(buffer_recv,"q")==0){
       close(sfd);
       break;
     }
     printf("Received message: %s\n",buffer_recv);
   }
 }

 else{
   if((sfd = socket(AF_INET,SOCK_DGRAM,0))==-1){
     perror("socket(): ");
     exit(0);
   }
   while(1){
     char buf[1024];
     printf("Enter message to send (q to quit): ");
     scanf("%s",buf);
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
}
