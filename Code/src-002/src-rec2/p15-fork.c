#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main(){
  pid_t  childpid;

  childpid = fork();

  if (childpid == -1){
     perror("Failed to fork");
     exit(1);
     }

  if (childpid == 0) 
     printf("I am the child process with ID: %lu \n", (long)getpid());
  else 
	{
	// sleep(3);
     	printf("I am the parent process with ID: %lu \n", 
			(long)getpid());
	}
  return 0;
}
    
