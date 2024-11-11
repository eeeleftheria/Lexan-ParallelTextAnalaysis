#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int main(){
   pid_t  pid;
   int status, exit_status,i ;

   if ( (pid = fork()) < 0 ) {
      	perror("fork failed"); exit(1);
	}

   if ( pid == 0 ){
	printf("Child %d starts sleeping... \n", getpid()); 
        sleep(5);
	printf("Child %d just finished sleeping... \n", getpid());
	exit(57);
        }

   printf("Reaching the father %lu process \n",(long)getpid());
   printf("PID of child is %lu \n", (long)pid);
   while( (waitpid(pid, &status, WNOHANG)) == 0 ){
        printf("Still waiting for child to return\n");
        sleep(1);
	}

   printf("Father %d process about to exit\n", getpid());

   if (WIFEXITED(status)){
       exit_status = WEXITSTATUS(status);
       printf("Exit status from %lu was %d\n", (long)pid, exit_status);
       }
   exit(0);
}
