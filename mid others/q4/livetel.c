#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>

int main(int argc,char *argv[])
{
	if(argc!=2)
	{
		printf("Enter port numnber\n");
		return 0;
	}

	int sfd = socket(AF_INET,SOCK_STREAM,0);

	if(sfd==-1)
	{
		perror("socket()");
	}

	struct sockaddr_in loc_addr,for_addr;
	loc_addr.sin_family = AF_INET;
	loc_addr.sin_addr.s_addr = INADDR_ANY; 
    loc_addr.sin_port = htons(atoi(argv[1])); 

    if(bind(sfd,(struct sockaddr *)&loc_addr,sizeof(loc_addr))<0)				{perror("bind()");}

    if (listen(sfd,4) < 0) 														{perror("listen"); } 
    

    int for_length = sizeof(for_addr);
    while(1){

        int nsfd = accept(sfd,(struct sockaddr *)&for_addr,&for_length);			if(nsfd<0){perror("accept()"); }

        while(1)
        {
        	char msg[1024];
        	fgets(msg,1024,stdin);
        	if(write(nsfd,msg,strlen(msg)+1)<0)		{perror("write()");}
        }
        close(nsfd);
    }
    close(sfd);
    return 0;


}