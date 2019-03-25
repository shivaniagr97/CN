#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
	int t;
	mkfifo("./well",0666);
	int pid=0;
	pid = getpid();
	int fd = open("./well",O_RDWR|O_NONBLOCK);
	char buf[12]={0};
	sprintf(buf,"%d",pid);
	buf[strlen(buf)]='\0';
	write(fd,buf,strlen(buf)+1);
	close(fd);

	char readfifo[12]={0};
	strcpy(readfifo,buf);
	strcat(readfifo,"r");
	mkfifo(readfifo,0666);

	char writefifo[12]={0};
	strcpy(writefifo,buf);
	strcat(writefifo,"w");
	mkfifo(writefifo,0666);

	printf("%s",buf);
	printf(readfifo);
	printf(writefifo);
	fflush(stdout);

	pid=fork();
	// char buf[12];
	while(1){
	if(pid>0){

		int fd=open(readfifo,O_RDWR|O_NONBLOCK);
		int n=0;
		while((n=read(fd,&buf,12))>0){
			buf[strlen(buf)-1]='\0';
			printf("%s\n",buf );
		}
		close(fd);
	}
	else if(pid==0){
		int i=0;
		for (i = 0; i < 12; ++i)buf[i]='\0';
		read(0,&buf,12);
		int fd=open(writefifo,O_RDWR|O_NONBLOCK);
		// printf("%d\n", fd);
		if(write(fd,&buf,12)<0) perror("write:");
		close(fd);
	}
	}
}
