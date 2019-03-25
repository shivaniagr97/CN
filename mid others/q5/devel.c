#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/select.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<ctype.h>
#include<signal.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<fcntl.h>

int main(int argc, char const *argv[])
{
	if(argc<2)
	{
		printf("enter port number.\n");
		return 0;
	}

	int sfd;
	struct sockaddr_in loc_addr,for_addr;

	if ( (sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
	
	for_addr.sin_family = AF_INET;
	for_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	for_addr.sin_port = htons(atoi(argv[1])); 

    if(connect(sfd,(struct sockaddr *)&for_addr,sizeof(for_addr))<0)
    {
    	perror("connect()");
    }

    while(1)
    {
    	char msg[1024];
    	char dport[100];
    	bzero(msg,1024);
		printf("Enter port:\n");
		scanf("%s",dport);
		strcpy(msg,dport);
		strcat(msg,"|");
		
		char dpath[100];
		printf("Enter path:\n");
		scanf("%s",dpath);
		strcat(msg,dpath);
		strcat(msg,"|");
		
		char desc[100];
		printf("Enter path:\n");
		scanf("%s",desc);
		strcat(msg,desc);
		strcat(msg,"|\0");
		sendto(sfd, msg, strlen(msg)+1, 
            MSG_CONFIRM, (const struct sockaddr *) &for_addr,  
                sizeof(for_addr));

    }
	return 0;
}