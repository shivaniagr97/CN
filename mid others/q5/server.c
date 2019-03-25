#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/select.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<pthread.h>
#include<ctype.h>
#include<signal.h>
#include<netdb.h>
#include<arpa/inet.h>
int sfd;
struct client
{
	int port;
	char addr[100];
	int nsfd;
}clients[15];
int c_c=0;
void *s2(void *par)
{
  int nsfd = *(int *)par;

  while(1)
  {
    char msg[100];
    int p = read(nsfd,msg,100);
    char s[100];
    if(p>0)
    {
    	printf("client:%d\n",p);
	    int len = strlen(msg);
	    sprintf(s,"%d",len);
	    write(nsfd,s,strlen(s));
    }
    
  }
}
void fn1()
{
	struct sockaddr_in for_addr;
	int for_length = sizeof(for_addr);
	printf("siged\n" );
	while(1)
	{
		//printf("server ip:%s  port:%d\n",inet_ntoa(serv.sin_addr),ntohs(serv.sin_port) )
		int nsfd = accept(sfd,(struct sockaddr *)&for_addr,(socklen_t *)&for_length);
		clients[c_c].port = ntohs(for_addr.sin_port);
		strcpy(clients[c_c].addr,inet_ntoa(for_addr.sin_addr));
		clients[c_c].nsfd = nsfd;
		c_c++;
		printf("%d\n",nsfd);
		pthread_t tid;
		pthread_create(&tid,NULL,s2,(void *)&nsfd);
	}
	
}
void fn2()
{
	for(int i=0;i<c_c;i++)
	{
		write(clients[i].nsfd,"new service is available:",strlen("new service is available:")+1);
	}
}
int main(int argc, char const *argv[])
{
	signal(SIGUSR1,fn1);
  	signal(SIGUSR2,fn2);
	if(argc<1)
	{
		printf("%s\n",argv[0]);
		return 0;
	}

	int port = atoi(argv[0]);
	
   	struct sockaddr_in loc_addr;
   	
	sfd = socket(AF_INET,SOCK_STREAM,0);         if(sfd<0){perror("socket()\n");}
      
	loc_addr.sin_family = AF_INET;
	loc_addr.sin_port = htons(port);
	loc_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int reuse = 1;
	
    int r = setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&reuse,sizeof(int));
    {perror("setsockopt()");}

	if(bind(sfd,(struct sockaddr *)&loc_addr,sizeof(loc_addr))<0){perror("bind()\n");}

	if(listen(sfd,20)<0)                      {perror("listen()\n");}
   
   
	printf("%s\n",argv[0]);
	while(1)
	{

	}

	return 0;
}