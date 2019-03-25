#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>

int main(int argc,char **argv){
  // int nsfdr;
  // int nsfdw;

  // printf("yo");
  // dup2(0,nsfdr);
  // dup2(1,nsfdw);
  //
  // freopen ("/dev/tty", "a", stdout);
  // freopen ("/dev/tty", "a", stdin);
  int nsfd = atoi(argv[1]);
  // printf("%d\n",nsfd );
  // fflush(stdout);
  // fflush(stdin);

  while(1){
    char buffer_recv[1024];
    int bytes_recv=0;

    if((bytes_recv=recv(nsfd,buffer_recv,1024,0))==-1){
      perror("recv(): ");
      exit(1);
    }
    buffer_recv[bytes_recv]='\0';
    if(strcmp(buffer_recv,"q")==0){
      // close(nsfdr);
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
    gets(buffer_send);
    // strcpy(buffer_send,"processing req");
    if(send(nsfd,buffer_send,strlen(buffer_send),0)==-1) perror("send");
    if(strcmp(buffer_send,"q")==0){
      close(nsfd);
      // close(nsfdw);
      fflush(stdin);
      fflush(stdout);
      exit(0);
    }
  }
}
