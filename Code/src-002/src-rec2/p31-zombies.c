#include <stdlib.h>
# include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void){ 
   pid_t pid;

   pid = fork();
   if ( pid == -1 ){ 
      perror("fork"); exit(1);
      }

   if ( pid!=0 ){
      while(1){ 	
		printf("Parent %d Process still alive\n",getpid());
		sleep(10); 	
		}
      	}
   else { 
	printf("Child process %d terminates!!!",getpid());
      	exit(37);
      	}
}
