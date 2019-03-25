#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/poll.h>
#include<sys/time.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include<string.h>
#include<sys/time.h>
#include<sys/poll.h>
#include<signal.h>
#include<sys/stat.h>
 
 
int main() 
{ 

    int key = ftok("prog",0);
  	msgid = msgget(key, 0666|IPC_CREAT);
	char buff[100];

	msgrcv(msgid2, &buff, sizeof(buff), getpid() , 0);
    
	fstream f;
	f.open("f1.txt",ios::in|ios::out);
	f<<buff;
    return 0; 
} 
