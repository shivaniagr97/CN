#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(){
  while(1){
    char buf[100]="";
    read(STDIN_FILENO,buf,100); //'/d' sends to document writer
    fflush(stdin);
    fflush(stdout);
    // char pid[20]="";
    // sprintf(pid,"%d",getpid());
    // strcat(buf,pid);
    printf("%s",buf);
    fflush(stdin);
    fflush(stdout);
  }
}
