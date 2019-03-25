#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/msg.h>
#include<fcntl.h>
struct mesg_buffer { 
    long type; 
    char text[100]; 
}dmsg;
int main()
{
	
	key_t key = ftok("doc",65);
	printf("%d\n",key );
	int dfd = msgget(key,0666 | IPC_CREAT);
	FILE* f = fopen("news.txt","a");
	int fd = fileno(f);
	while(1)
	{
		msgrcv(dfd,&dmsg,sizeof(dmsg),0,0);
		write(fd,dmsg.text,strlen(dmsg.text)+1);
		printf("%s\n",dmsg.text);
		write(fd,"\n",1);
	}
	close(fd);
	return 0;

}