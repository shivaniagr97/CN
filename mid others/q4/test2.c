#include<stdio.h>
#include<signal.h>
int main()
{
	int pid ;
	scanf("%d",&pid);
	kill(pid,SIGUSR1);
	scanf("%d",&pid);
	kill(pid,SIGUSR2);
}