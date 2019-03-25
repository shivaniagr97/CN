#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/msg.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<signal.h>
#include<ctype.h>
struct mesg_buffer { 
    long type; 
    char text[100]; 
}msg,lmsg;

int main()
{
	key_t key = ftok("newsreader",65);
	printf("%d\n",key );
	key_t key2 = ftok("scr",65);
	printf("%d\n",key2 );
	int dfd = msgget(key,0666 | IPC_CREAT);
	int scr = msgget(key2,0666 | IPC_CREAT);

	int f = 0;

	msgrcv(dfd,&msg,sizeof(msg),1,0);
	int pid = atoi(msg.text);

	while(1)
	{
		msgrcv(dfd,&msg,sizeof(msg),1,0);
		if(isdigit(msg.text[0])||f)
		{
			kill(pid,SIGUSR1);

			struct sockaddr_in loc_addr,for_addr;
			int sfd = socket(AF_INET,SOCK_STREAM,0);		if(sfd==-1){perror("socket()");}
			
			for_addr.sin_family = AF_INET;
			for_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
			for_addr.sin_port = htons(atoi(msg.text));

			if(connect(sfd,(struct sockaddr *)&for_addr,sizeof(for_addr))<0)
		    {
		    	perror("connect()");
		    }

		    while(1)
		    {
		    	bzero(lmsg.text,100);
		    	if(read(sfd,lmsg.text,100)<0)					{perror("read()");}
		    	printf("live: %s",lmsg.text);
		    	lmsg.type = 1;
		    	msgsnd(scr,&lmsg,sizeof(lmsg),0);
		    	if(lmsg.text[0]=='$')
		    	{
		    		break;
		    	}
		    }
    		close(sfd);
			kill(pid,SIGUSR2);
		}
		else
		{
			msg.type=1;
			msgsnd(scr,&msg,sizeof(msg),0);
		}
	}
}