#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>

int main()
{
	int fd = open("just.txt",O_WRONLY|O_APPEND);
	write(fd,"jshdf\n",strlen("jshdf\n"));
	write(fd,"nijf\n",strlen("nijf\n"));
}
