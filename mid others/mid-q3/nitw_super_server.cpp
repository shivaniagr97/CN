#include<bits/stdc++.h>
#include<string.h>
#include <string.h> 
#include <stdlib.h> 
#include <errno.h> 
#include<signal.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/shm.h> 
#include<fcntl.h>
#include <sys/time.h> 
using namespace std;

#define TRUE 1 
#define FALSE 0 

struct inetel{
	int port,no;
	string prot;
	string file;
	string type;
	vector<int>v;
	int pid[100];
	int l;
};

struct val{
	int from;
	int nsfd;
	int pid;
};

int fp;
int t=0;
struct sockaddr_in address,client; 
struct inetel it[100];
int sfd[100];



void fn2(int x){
	key_t key1 = 1235;
	int shmid1 = shmget(key1, sizeof(struct val), IPC_CREAT | 0666);
	val *p2 = (val*)shmat(shmid1, NULL, 0);
	for(int i=0;i<it[p2->from].no;i++){
		if(it[p2->from].pid[i]==p2->pid){
			swap(it[p2->from].pid[i],it[p2->from].pid[it[p2->from].no-1]);
			it[p2->from].l--;
		}
	}
	it[p2->from].v.erase(find(it[p2->from].v.begin(),it[p2->from].v.end(),p2->nsfd));
}

int tt;
void* thread_pr(void* v){
	int i = tt;
	int nsfd;
	char str[100];
	int addrlen = sizeof(address);
	
	if((nsfd = accept(sfd[i],(struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {cout<<"errorr!!";}
	it[i].v.push_back(nsfd);	
	printf("ServerT : ");
	scanf("%s",str);
			
	send(nsfd,str,100,0);
	
	while(1){
		recv(nsfd,str,100,0);
				
		if(strcmp(str,"q")==0||strcmp(str,"Q")==0){
			strcpy(str,"Client Bye!!");
			send(nsfd,str,100,0);
			it[i].v.erase(find(it[i].v.begin(),it[i].v.end(),nsfd));
			close(nsfd);
			return NULL;
		}
		int l = strlen(str);
		sprintf(str, "%d", l);
		send(nsfd,str,100,0);
	}
}

long pid;

int main(int argc , char *argv[]) 
{ 
	string buf2,buf1;
	fstream f;
	f.open("f1.txt",ios::in|ios::out);

	fd_set readfds;
	key_t key;
	int shmid;
	 
	key = 5678;
	shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
	int *v1 = (int*)shmat(shmid, NULL, 0);

	key_t key1;
	int shmid1;
	 
	key1 = 1234;
	shmid1 = shmget(key1, sizeof(int), IPC_CREAT | 0666);
	int *p1 = (int*)shmat(shmid1, NULL, 0);
	
	*p1 = getpid();
	pid = *p1;

	signal(SIGUSR1,(void (*)(int))fn2);

	while(!f.eof()){
		getline(f,buf1);
		buf2="";
		int b=0,k=0;
		for(int i=0;i<buf1.length();i++){
			if(buf1[i]=='$'&&k==0){
				k++;
				it[t].file = buf2;
				buf2="";
			}
			else if(buf1[i]=='$' && k==1){
				k++;
				it[t].prot = buf2;
				buf2="";
			}
			else if(buf1[i]=='$' && k==2){
				k++;
				it[t].port = stoi(buf2,nullptr,10);
				buf2="";
			}
			else if(buf1[i]=='$' && k==3){
				k++;
				it[t].no = stoi(buf2,nullptr,10);
				buf2="";
			}
			else if(buf1[i]=='$' && k==4){
				k++;
				it[t].type = buf2;
				buf2="";
			}
			else{
				buf2+=buf1[i];
			}
		}
		if(it[t].port==0)
			break;
		if(it[t].prot=="TCP"){
			sfd[t] = socket(AF_INET , SOCK_STREAM , 0);	
		}
		else{
			sfd[t] = socket(AF_INET , SOCK_DGRAM , 0);	
		}
		address.sin_family = AF_INET; 
		address.sin_addr.s_addr = INADDR_ANY; 
		address.sin_port = htons( it[t].port );
		
		cout<<it[t].prot<<"\t"<<it[t].port<<"\t"<<it[t].file<<"\t"<<it[t].no<<"\t"<<it[t].type<<endl;
		
		if (bind(sfd[t], (struct sockaddr *)&address, sizeof(address))<0) 
		{ 
			perror("bind failed"); 
			exit(EXIT_FAILURE); 
		} 
		if(it[t].prot=="TCP"){
			listen(sfd[t], 3);	
		}
		for(int i=0;i<it[t].no;i++){
			int c = fork();
			if(c==0){
				char* args[2]={NULL,NULL};
				args[0] = strdup(it[t].file.c_str());
				execv(args[0],args);
			}
			else{
				sleep(2);
				key_t key1 = 1237;
				int shmid1 = shmget(key1, sizeof(int), IPC_CREAT | 0666);
				int *p2 = (int*)shmat(shmid1, NULL, 0);
				it[t].pid[i] = *p2;
			}
		}
		it[t].l =0;
		t++;
	}
	int addrlen = sizeof(address); 
	puts("Waiting for connections ..."); 
	
	int max_sd = -1;
	while(TRUE) 
	{ 
		FD_ZERO(&readfds); 
		
		for(int i=0;i<t;i++){
			FD_SET(sfd[i], &readfds); 
			if(max_sd<sfd[i])
				max_sd = sfd[i]; 
		}
	
		int activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); 
	
		if ((activity < 0) && (errno!=EINTR)) 
		{ 
			printf("select error"); 
		} 
		
		int nsfd;
		
		for(int i=0;i<t;i++){
			if (FD_ISSET(sfd[i], &readfds)) 
			{ 
				char* args[2]={NULL,NULL};
				args[0] = strdup(it[i].file.c_str());
				cout<<"Connected to server "<<it[i].v.size();
				if (it[i].prot=="TCP") 
				{ 
					
					if(it[i].v.size()<it[i].no){
						if(it[i].type=="process"){
							if((nsfd = accept(sfd[i],(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)	break;
							printf("Accepted..");
							fflush(stdout);
							it[i].v.push_back(nsfd);
							*v1 = nsfd;
							
							kill(it[i].pid[it[i].l++],SIGINT);
							
						}
						else{
							pthread_t tid;
							tt = i;
							char v;
							pthread_create(&tid,NULL,thread_pr,&v);
						}
					}
					else{
						char bf[100];
						strcpy(bf,"Server is busy...");
						nsfd = accept(sfd[i],(struct sockaddr *)&address, (socklen_t*)&addrlen);
						send(nsfd,bf,100,0);
						close(nsfd);
					}
				}
					
				else{
					//udpfn(i);
				}
			} 
				
		}	
		
	} 
	return 0; 
} 

