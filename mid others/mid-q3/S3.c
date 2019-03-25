#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include<sys/shm.h>

int main(){
	
	while(1){
		
		key_t key;
		key = 5678;
		int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
		int *v1 = shmat(shmid, NULL, 0);
 		
 		int nsfd = *v1;

 		char str[100];
		while(1){
			printf("Server3 : ");
			scanf("%s",str);
			
			send(nsfd,str,100,0);
			
			recv(nsfd,str,100,0);
			printf("Client : %s\n",str);
			
			if(strcmp(str,"q")==0||strcmp(str,"Q")==0){
				strcpy(str,"Server Quitting!!");
				send(nsfd,str,100,0);
				close(nsfd);
				break;
			}
		}
		
	}
	
	return 0;
}
	
