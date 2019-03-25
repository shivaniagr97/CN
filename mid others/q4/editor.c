#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/msg.h>
#include<fcntl.h>
#include<signal.h>
struct mesg_buffer { 
    long type; 
    char text[100]; 
}dmsg,nmsg;
int s=0;
void fn1()
{
	s=1;
	while(s){}
}
void fn2()
{
	s=0;
}
int main()
{
	int pid = getpid();
	signal(SIGUSR1,fn1);
	signal(SIGUSR2,fn2);

	key_t key = ftok("doc",65);
	printf("%d\n",key);
	int dfd = msgget(key,0666 | IPC_CREAT);
	
	key_t key1 = ftok("newsreader",65);
	printf("%d\n",key1 );
	int nfd = msgget(key1,0666 | IPC_CREAT);

	for(int i=1;i<=2;i++)
	{
		dmsg.type = i;
		sprintf(dmsg.text,"%d",pid);
		msgsnd(nfd,&dmsg,sizeof(dmsg),0);

	}

	mkfifo("rep1",0666);
	mkfifo("rep2",0666);
	mkfifo("rep3",0666);
	int rfd[3],f[3];
	rfd[0] = open("rep1",O_RDONLY);
	rfd[1] = open("rep2",O_RDONLY);
	rfd[2] = open("rep3",O_RDONLY);
	f[0]=f[1]=f[2]=0;
	fd_set rfds;
	int retval,nc=0;
	while(1)
	{
		FD_ZERO(&rfds);
		for(int i=0;i<3;i++)
		{
			FD_SET(rfd[i],&rfds);
		}

		retval = select(FD_SETSIZE,&rfds,NULL,NULL,NULL);

		for(int i=0;i<3;i++)
		{
			if(FD_ISSET(rfd[i],&rfds))
			{
				char msg[1024];
				bzero(msg,1024);
				read(rfd[i],msg,1024);
				//printf("%d\n",strlen(msg));
				//printf("%s\n",msg);
				if(msg[0]=='$')
				{
					f[i]=0;
				}
				else if(msg[0]=='/'&&msg[1]=='d'||f[i])
				{
					
					dmsg.type = i+1;
					strcpy(dmsg.text,msg);
					//printf("send:%s\n",dmsg.text);
					int x=msgsnd(dfd,&dmsg,sizeof(dmsg),0);
					//printf("%d\n",x);
					f[i]=1;
				}
				else 
				{
					if(!nc)
					{
						nc++;nmsg.type=1;
						strcpy(nmsg.text,msg);
						msgsnd(nfd,&nmsg,sizeof(nmsg),0);
					}
					else
					{
						nc--;nmsg.type=2;
						strcpy(nmsg.text,msg);
						msgsnd(nfd,&nmsg,sizeof(nmsg),0);
					}
				}
			}
		}
	}

}
