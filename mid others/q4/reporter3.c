#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/msg.h>
#include<fcntl.h>

int main()
{
	mkfifo("rep3",0666);
	int pfd = open("rep3",O_WRONLY);

	printf("enter news\n");
	dup2(pfd,1);
	while(1)
	{
		char msg[1024];bzero(msg,1024);
		scanf("%s",msg);
		printf("%s",msg);
	}
	
}