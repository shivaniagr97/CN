#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<signal.h>
int main(int argc,char *argv[])
{
	struct sockaddr_in loc_addr,for_addr;
	if(argc<2)
	{
		printf("Enter port numnber\n");
		return 0;
	}
	

	int sfd = socket(AF_INET,SOCK_STREAM,0);		if(sfd==-1){perror("socket()");}
	
	
	for_addr.sin_family = AF_INET;
	for_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    for_addr.sin_port = htons(atoi(argv[1])); 

    if(connect(sfd,(struct sockaddr *)&for_addr,(socklen_t)sizeof(for_addr))<0)
    {
    	perror("connect()");
    }
    while(1)
    {
    	char msg[1024];
    	scanf("%s",msg);
    	if(send(sfd,msg,strlen(msg)+1,0)<0)			{perror("write()");}
    	bzero(msg,1024);
    	if(read(sfd,msg,8)<0){perror("read()");}
    	printf("server: %s",msg);
    }
    
    return 0;
}
