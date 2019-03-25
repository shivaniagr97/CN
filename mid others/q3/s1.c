#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<ctype.h>
#include <signal.h>
#include <stdlib.h>
int main()
{
	while(1)
	{
		char msg[100];
		read(2,msg,100);
		if(isdigit(msg[0]))
		{
			kill(atoi(msg),SIGUSR1);
			break;
		}
		printf("%s",msg);
		scanf("%s",msg);
		write(3,msg,strlen(msg)+1);
	}
}