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
struct service_port{
	int port;
	int sfd;
	int pid;
	char description[100];
	char path[100];
}s_ports[25];


int s_c=0;
int main(int argc, char const *argv[])
{
	if(argc<2)
	{
		printf("enter port number.\n");
		return 0;
	}

	int ufd;
	struct sockaddr_in loc_addr,for_addr;
	if((ufd = socket(AF_INET,SOCK_DGRAM,0))<0){perror("connectionless socket()");exit(EXIT_FAILURE);}

    loc_addr.sin_family = AF_INET;
    loc_addr.sin_addr.s_addr = INADDR_ANY;
    loc_addr.sin_port = htons(atoi(argv[1]));

    s_ports[0].port = atoi(argv[1]);
    s_ports[0].sfd = ufd;
    strcpy(s_ports[0].description,"well known port");
    s_c++;

    if(bind(ufd,(const struct sockaddr *)&loc_addr,sizeof(loc_addr))<0)
    {
    	perror("connectionless bind failed.");
    	exit(EXIT_FAILURE);
    }
    
    fd_set rfds;
    int retval;

    while(1)
    {
    	FD_ZERO(&rfds);
		for(int i=0;i<s_c;i++)
		{
			FD_SET(s_ports[i].sfd,&rfds);
		}

		retval = select(FD_SETSIZE,&rfds,NULL,NULL,NULL);

		if(retval==-1){perror("select()");exit(EXIT_FAILURE);}
		else if(retval)
		{
			//printf("ojjj\n");
			if(FD_ISSET(s_ports[0].sfd,&rfds))
			{
				int len;
				char msg[1024];
				recvfrom(s_ports[0].sfd,msg,1024,MSG_WAITALL,(struct sockaddr *)&for_addr,&len);
				if(isdigit(msg[0]))
				{
					// get port path and description
					int i=0,j=0;
					char dport[100];
					printf("%s\n",msg);
					while(msg[i]!='|')
					{
						dport[j++]=msg[i++];
					}
					dport[j]='\0';

					s_ports[s_c].port = atoi(dport);
					j=0;
					i++;
					char dpath[100];
					while(msg[i]!='|')
					{
						dpath[j++]=msg[i++];
					}
					dpath[j]='\0';
					strcpy(s_ports[s_c].path,dpath);
					j=0;
					i++;
					char desc[100];
					while(msg[i]!='|')
					{
						desc[j++]=msg[i++];
					}
					desc[j]='\0';
					strcpy(s_ports[s_c].description,desc);

					//write in a file
					int pfd = open("inetd.txt",O_WRONLY|O_APPEND);
					write(pfd,dport,strlen(dport));
					write(pfd," ",1);
					write(pfd,dpath,strlen(dpath));
					write(pfd," ",1);
					write(pfd,desc,strlen(desc));
					write(pfd,"\n",1);
					close(pfd);
					//create sfd
					struct sockaddr_in loc_addr1;
   					struct sockaddr_in for_addr1;

   					s_ports[s_c].sfd = socket(AF_INET,SOCK_STREAM,0);         if(s_ports[s_c].sfd<0){perror("socket()\n");}
      				perror("socket()\n");
			        loc_addr1.sin_family = AF_INET;
			        loc_addr1.sin_port = htons(atoi(dport));
			        loc_addr1.sin_addr.s_addr = inet_addr("127.0.0.1");

					int reuse = 1;
	
				    int r = setsockopt(s_ports[s_c].sfd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&reuse,sizeof(int));
				    {perror("setsockopt()");}
			        if(bind(s_ports[s_c].sfd,(struct sockaddr *)&loc_addr1,sizeof(loc_addr1))<0){perror("bind()\n");}

			        //printf("server ip:%s  port:%d\n",inet_ntoa(serv.sin_addr),ntohs(serv.sin_port) )
			   
			        if(listen(s_ports[s_c].sfd,20)<0)                      {perror("listen()\n");}
			        //send messages:
			        for(int i=1;i<s_c;i++)
			        {
			        	kill(s_ports[i].pid,SIGUSR2);
			        }

			        int c = fork();
			        if(c)
			        {
			        	s_ports[s_c].pid = c;
			        	s_c++;
			        }
			        else
			        {
			        	for(int i=0;i<s_c;i++)
			        	{
			        		close(s_ports[i].sfd);
			        	}
			        	char id[10];
			        	sprintf(id,"%d",s_ports[s_c].sfd);
			        	char *tem[]={dport,NULL};
			        	execvp(dpath,tem);
			        }

				}
				else
				{
					int pfd = open("inetd.txt",O_RDONLY);
					char ch[1],to[1024];
					int i=0;
					while(read(pfd,ch,1))
					{
						to[i++]=ch[0];
					}
					to[i]='\0';
					sendto(ufd,to,strlen(to)+1,MSG_CONFIRM, (const struct sockaddr *)&for_addr,len);

				}
			}
			//else
			
				
				for(int i=1;i<s_c;i++)
				{
					//printf("%d\n",s_c);
					if(FD_ISSET(s_ports[i].sfd,&rfds))
					{
						//printf("%d\n",s_ports[i].pid);
						kill(s_ports[i].pid,SIGUSR1);
						printf("killed\n");
					}
				}
		
		}
    }

	return 0;
}