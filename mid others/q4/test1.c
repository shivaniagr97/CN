#include<stdio.h>
#include<signal.h>
#include <unistd.h>
int s=0;
void fn1()
{
	s=1;
	while(s){}
}
void fn2()
{
	s=0;
}
int main()
{
	signal(SIGUSR1,fn1);
	signal(SIGUSR2,fn2);
	printf("%d\n",getpid());
	while(1)
	{
		printf("%d\n",getpid());
	}
}