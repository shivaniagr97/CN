#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

int fdr[100]={0};
int r=0;
int fdw[100]={0};
int w=1;

void sigint(int sig){
  for(int i=0;i<r;i++) close(fdr[i]);
  for(int i=1;i<w;i++) close(fdw[i]);
  exit(0);
}

int findMax(){
  int max = 0;
  for(int i=0;i<r;i++){
    if(max<fdr[i]) max=fdr[i];
  }
  return max;
}

int main(){

  mkfifo("./well",0666);
  int pp = open("./well",O_RDWR|O_NONBLOCK);
  fdr[r++] = pp;
  // char buf[12];
  // while(read(pp,buf,12)<0);
  // printf(buf);
  // close(pp);

  fd_set rfds;
  struct timeval tv;
  int retval,wr;

  int i,flag;
  tv.tv_sec=5;
  tv.tv_usec=0;
  char buffer[12]={0};
  retval=0;

  while(1){
    sleep(1);
    // fflush(stdout);

    do{
      FD_ZERO(&rfds);
      for(int j=0;j<r;j++){
        FD_SET(fdr[j],&rfds);
      }
      int maxfd = findMax();
      retval=select(maxfd+1,&rfds,NULL,NULL,&tv);
      // printf("%d\n",retval );
    }while(retval==-1);

    for(int i=1;i<r;i++){
      if(FD_ISSET(fdr[i],&rfds)){
        read(fdr[i],buffer,12);
        buffer[strlen(buffer)]='\0';
        printf("Client %d messaged: %s\n",i,buffer);
        for(int j=1;j<w;j++){
          if(j!=i) write(fdw[j],buffer,strlen(buffer));
        }
      }
    }

    if(FD_ISSET(fdr[0],&rfds)){
      char buf[12]={0};
      read(pp,buf,12);
      printf(buf);

      char readfifo[12]={0};
    	strcpy(readfifo,buf);
    	strcat(readfifo,"w");
    	mkfifo(readfifo,0666);

    	char writefifo[12]={0};
    	strcpy(writefifo,buf);
    	strcat(writefifo,"r");
    	mkfifo(writefifo,0666);

      fflush(stdout);
      printf("%s",buf);
    	printf(readfifo);
    	printf(writefifo);
    	fflush(stdout);


      fdr[r++] = open(readfifo,O_RDWR|O_NONBLOCK);
      fdw[w++] = open(writefifo,O_RDWR|O_NONBLOCK);
    }
  }
}
