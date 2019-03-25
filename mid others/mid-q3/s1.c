#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/shm.h>
#include<signal.h>

struct val{
	int from;
	int nsfd;
	int pid;
};
int *p1,*v1;
struct val *p3;
void fn(){
	char str[100];
	int nsfd = *v1;
	while(1){
			
			printf("Server1 : ");
			scanf("%s",str);
			
			send(nsfd,str,100,0);

			recv(nsfd,str,100,0);
			printf("Client : %s\n",str);
			
			if(strcmp(str,"q")==0||strcmp(str,"Q")==0){
				strcpy(str,"Client Bye!!");
				send(nsfd,str,100,0);
				p3->nsfd = nsfd;
				kill(*p1,SIGUSR1);
				close(nsfd);
				break;
			}
			
		}
}

int main(){
	
	signal(SIGINT,fn);
	printf("Welcome...");
	fflush(stdout);
	key_t key1 = 1237;
	int shmid1 = shmget(key1, sizeof(int), IPC_CREAT | 0666);
	int *p2 = shmat(shmid1, NULL, 0);

	*p2 = getpid();
	key_t key;
		key = 5678;
		int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
		v1 = shmat(shmid, NULL, 0);
 		*v1=-1;

	while(1){
		
		
		key_t key1 = 1234;
		int shmid1 = shmget(key1, sizeof(int), IPC_CREAT | 0666);
		p1 = (int*)shmat(shmid1, NULL, 0);

		key_t key2 = 1235;
		int shmid2 = shmget(key1, sizeof(struct val), IPC_CREAT | 0666);
		struct val *p2 = (struct val*)shmat(shmid1, NULL, 0);

 		p2->from = 1;
		p2->pid = getpid();
 		
 		p3 = p2;
		
		while(*v1==-1){
			key_t key;
			key = 5678;
			int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
			v1 = shmat(shmid, NULL, 0);
	 	}
	 	fn();
		
	}
	
	return 0;
}
	
