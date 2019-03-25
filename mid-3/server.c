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

struct config{
  char serviceName[20];
  char protocol[20];
  int port;
  char type[20];
  int limit;
  char serviceOfferedBy[20];
}services[100];

int nos=0;
int sfd[100]={0};


//signal handler to close all sockets on exit
void sigint(int sig){
  for(int i=0;i<nos;i++){
    close(sfd[i]);
  }
  exit(0);
}

//copying from buffer to structure
void copyToStruct(char token[20],int j){
  if(j==0) strcpy(services[nos].serviceName,token);
  else if(j==1) strcpy(services[nos].protocol,token);
  else if(j==2){
    services[nos].port = atoi(token);
  }
  else if(j==3) strcpy(services[nos].type,token);
  else if(j==4){
    services[nos].limit = atoi(token);
  }
  else if(j==5) strcpy(services[nos++].serviceOfferedBy,token);
}

//reading from file
void initServices(){
  FILE* fptr = fopen("config.txt","r");
  char *buf;
  size_t bufsize=1024;
  buf=(char*)malloc(bufsize*sizeof(char));

  while(getline(&buf,&bufsize,fptr)!=-1){
    buf[strlen(buf)-1]='\0';
    size_t len =0;
    int i = 0;
    int j=0;
    char* token = strtok(buf, "|");
    while (token != NULL) {
        copyToStruct(token,j++);
        token = strtok(NULL, "|");
    }

  }
  fclose(fptr);
}

//find max sfd to pass in select
int findMax(int sfd[],int n){
  int max = -1;
  for(int i=0;i<n;i++){
    if(max<sfd[i]) max=sfd[i];
  }
  return max;
}

void * serviceThread(void *arg){
  int *nsfd = (int *)arg;
  while(1){
    char buffer_recv[1024];
    int bytes_recv=0;

    // printf("hi");
    fflush(stdout);
    if((bytes_recv=recv(*nsfd,buffer_recv,1024,0))==-1){
      perror("recv(): ");
      pthread_exit(0);
    }
    buffer_recv[bytes_recv]='\0';
    if(strcmp(buffer_recv,"q")==0){
      close(*nsfd);
      fflush(stdin);
      fflush(stdout);
      pthread_exit(0);
    }
    printf("message Received: %s\n",buffer_recv);
    fflush(stdout);

    char buffer_send[1024];
    printf("Enter message to send /press q to quit: ");
    fflush(stdout);
    scanf("%s",buffer_send);
    if(send(*nsfd,buffer_send,strlen(buffer_send),0)==-1) perror("send");
    if(strcmp(buffer_send,"q")==0){
      close(*nsfd);
      fflush(stdin);
      fflush(stdout);
      pthread_exit(0);
    }
  }
}

void udpfunc(int sfd){
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

int main(){

  signal(SIGINT,sigint); //close all sockets on exit

  initServices(); //copying file to struct

  //printing contents of services
  printf("Services offered initially: \n");
  for(int i=0;i<nos;i++){
    printf("%s %s %d %s %d %s\n", services[i].serviceName, services[i].protocol, services[i].port, services[i].type, services[i].limit, services[i].serviceOfferedBy);
  }

  for(int i=0;i<nos;i++){

    //creating socket for connection oriented
    if(strcmp(services[i].protocol,"TCP")==0){
      if((sfd[i] = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("socket(): ");
        exit(1);
      }
      struct sockaddr_in serveraddr;
      serveraddr.sin_family = AF_INET;
      serveraddr.sin_port = htons(services[i].port);
      serveraddr.sin_addr.s_addr = INADDR_ANY;
      bzero((void *)&serveraddr.sin_zero,8);
      if(bind(sfd[i],(struct sockaddr *)&serveraddr,sizeof(serveraddr))==-1){
        perror("bind(): ");
        exit(1);
      }
      if(listen(sfd[i],20)==-1){
        perror("listen(): ");
        exit(1);
      }
    }

    //creating socket to take requests for UDP, threads etc
    else{
      if((sfd[i] = socket(AF_INET,SOCK_DGRAM,0))==-1){
        perror("socket(): ");
        exit(1);
      }
      struct sockaddr_in serveraddr;
      serveraddr.sin_family = AF_INET;
      serveraddr.sin_port = htons(services[i].port);
      serveraddr.sin_addr.s_addr = INADDR_ANY;
      bzero((void *)&serveraddr.sin_zero,8);
      if(bind(sfd[i],(struct sockaddr *)&serveraddr,sizeof(serveraddr))==-1){
        perror("bind(): ");
        exit(1);
      }
    }
  }

  fd_set rfds;
  struct timeval tv;
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  while(1){
    FD_ZERO(&rfds);
    for(int i=0;i<nos;i++){
      FD_SET(sfd[i],&rfds);
    }
    int max = findMax(sfd,nos);
    int retval = select(max+1,&rfds,NULL,NULL,&tv);
    if(retval==-1){
      perror("select(): ");
      exit(1);
    }
    //when there is a waiting request
    else if(retval>0){
      for(int i=0;i<nos;i++){
        if(FD_ISSET(sfd[i],&rfds)){
          //if the request is for connection oriented
          if(strcmp(services[i].protocol,"TCP")==0){

            struct sockaddr_in client_addr;
            int len=sizeof(struct sockaddr_in);
            int nsfd = accept(sfd[i],(struct sockaddr *)&client_addr,&len);

            //if External then create a new process
            if(strcmp(services[i].type,"External")==0){
              int pid = fork();
              if(pid==0){
                for(int i=0;i<nos;i++) close(sfd[i]);
                char nsfdbuf[20]="";
                sprintf(nsfdbuf,"%d",nsfd);
                char *args[]={services[i].serviceName,nsfdbuf,NULL};
                execv(args[0],args);
              }
              else if(pid>0){
                close(nsfd);
              }
              else{
                perror("fork(): ");
                exit(1);
              }
            }
            //tcp service provided by thread
            else if(strcmp(services[i].serviceOfferedBy,"thread")==0){
              pthread_t t;
              pthread_create(&t,NULL,(void *)&serviceThread,(void *)&nsfd);
            }
          }

          if(strcmp(services[i].protocol,"UDP")==0){

            int pid = fork();
            if(pid==0){
              udpfunc(sfd[i]);
            }

          }
        }
      }
    }
  }
}
