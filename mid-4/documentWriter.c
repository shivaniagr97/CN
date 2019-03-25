#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(){
  char buf[100]="";
  read(STDIN_FILENO,buf,100);
  int fd = open("document.txt",O_RDWR|O_APPEND);
  write(fd,buf,strlen(buf));
  close(STDIN_FILENO);
  close(fd);
  return 0;
}
