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
}msg;

int main()
{
	key_t key = ftok("scr",65);
	printf("%d\n",key );
	int scr = msgget(key,0666 | IPC_CREAT);
	while(1){
		msgrcv(scr,&msg,sizeof(msg),0,0);
		printf("%s\n",msg.text);
	}
}