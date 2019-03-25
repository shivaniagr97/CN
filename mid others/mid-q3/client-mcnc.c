#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


	
int main(){

	int true = 1;
	struct sockaddr_in server,client;
	struct hostent *host;
	
	char buf[100];
	
	printf("Enter server u want to connect ... ");
	int c;
	scanf("%d",&c);
	
	struct sockaddr_in server_addr;
	host = gethostbyname("127.0.0.1");
	server.sin_family = AF_INET;     
    server.sin_addr = *((struct in_addr *)host->h_addr);
    
	printf("Enter the port u want to connect to?");
	int p;
	scanf("%d",&p);
	server.sin_port = htons( p );

	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	
    bzero(&(server.sin_zero),8); 

	connect(sfd,(struct sockaddr*)&server,sizeof(struct sockaddr));	
	
	while(1){

		recv(sfd,buf,100,0);
		if(strcmp(buf,"Server Quitting!!")==0||strcmp(buf,"Server is busy...")==0){
			close(sfd);
			break;
		}
	
		printf("Server : %s\n",buf);

		
		printf("Client : ");
		scanf("%s",buf);
		send(sfd,buf,100,0);
		
	}
	
	return 0;
}
	
