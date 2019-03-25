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
#include <pthread.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<fcntl.h>
void *readfn(void *param)
{
	int nsfd = *(int *)param;
	while(1)
	{
		printf("in thread:\n");
		char msg1[100];
		bzero(msg1,100);
		read(nsfd,msg1,100);
		printf("%s\n",msg1);
	}
}

int main(int argc,void *argv[])
{
	if(argc<2)
    {
        printf("Enter port\n");
        return 0;
    }

    int sfd;
    printf("Enter client's port:\n");
    int port;
    scanf("%d",&port);
    if ( (sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 

    struct sockaddr_in  for_addr,loc_addr; 
    for_addr.sin_family = AF_INET; 
    for_addr.sin_port = htons(atoi(argv[1])); 
    for_addr.sin_addr.s_addr = INADDR_ANY; 
    printf("Enter message:\n"); 
        char list[1024];
        int len;
        scanf("%s",list);
        sendto(sfd, list, strlen(list)+1, 
            MSG_CONFIRM, (const struct sockaddr *) &for_addr,  
                sizeof(for_addr)); 
        
        bzero(list,1024);
       int n = recvfrom(sfd, (char *)list, 1024,  
                    MSG_WAITALL, (struct sockaddr *) &for_addr, 
                    &len); 
        list[n] = '\0'; 
        printf("Server : %s\n", list);

    printf("Enter service port:\n");
    int port1;
    scanf("%d",&port1);
    close(sfd);

    sfd = socket(AF_INET,SOCK_STREAM,0);

    loc_addr.sin_family = AF_INET;
    loc_addr.sin_addr.s_addr = INADDR_ANY;
    loc_addr.sin_port = htons(port);

    
	

    int reuse = 1;
    int r = setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&reuse,sizeof(int));
    if(r<0){perror("setsockopt()");exit(EXIT_FAILURE);}

    if(bind(sfd,(const struct sockaddr *)&loc_addr,sizeof(loc_addr))<0)
    {
    	perror("connectionless bind failed.");
    	exit(EXIT_FAILURE);
    }


    for_addr.sin_family = AF_INET;
	for_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	for_addr.sin_port = htons(port1); 

	if(connect(sfd,(struct sockaddr *)&for_addr,sizeof(for_addr))<0)
    {
    	perror("connect()");
    }


    pthread_t t;
    
    pthread_create(&t,NULL,readfn,(void *)&sfd);

    while(1)
	{
		char msg[100];
		bzero(msg,100);
		printf("Enter text:\n");
		scanf("%s",msg);
		write(sfd,msg,strlen(msg)+1);
	}

}
