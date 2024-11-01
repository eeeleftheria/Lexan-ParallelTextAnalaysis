#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void){
   pid_t pid;

   printf("Original process: PID=%d, PPID=%d\n", getpid(), getppid());

   pid = fork();
   if ( pid == -1 ){
      	perror("fork");
      	exit(1);
      	}
   if ( pid != 0 ){
      	printf("Parent process: PID=%d, PPID=%d, CPID=%d\n", \
				getpid(), getppid(), pid);
	printf("I am getting out-Bye from Parent!\n"); 
	fflush(stdout);
	}
   else {
      	printf("Child process: PID=%d, PPID=%d\n",  \
				getpid(), getppid());
      	sleep(3);
      	}
   printf("\nProcess with PID=%d terminates - Parent is: %d\n", 
				getpid(), getppid());
}
