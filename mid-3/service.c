#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>

int main(int argc,char **argv){
  int nsfd = atoi(argv[1]);
  fflush(stdout);
  fflush(stdin);
  while(1){
    char buffer_recv[1024];
    int bytes_recv=0;

    // printf("hi");
    fflush(stdout);
    if((bytes_recv=recv(nsfd,buffer_recv,1024,0))==-1){
      perror("recv(): ");
      exit(1);
    }
    buffer_recv[bytes_recv]='\0';
    if(strcmp(buffer_recv,"q")==0){
      close(nsfd);
      fflush(stdin);
      fflush(stdout);
      exit(0);
    }
    printf("message Received: %s\n",buffer_recv);
    fflush(stdout);

    char buffer_send[1024];
    printf("Enter message to send /press q to quit: ");
    fflush(stdout);
    scanf("%s",buffer_send);
    if(send(nsfd,buffer_send,strlen(buffer_send),0)==-1) perror("send");
    if(strcmp(buffer_send,"q")==0){
      close(nsfd);
      fflush(stdin);
      fflush(stdout);
      exit(0);
    }
  }
}
