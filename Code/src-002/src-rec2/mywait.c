#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
  pid_t  pid;
  int  status, exit_status;

  if ( (pid = fork()) < 0 )
	perror("fork failed");

  if (pid==0){
     sleep(4);
     exit(5); /* exit with non-zero value */
     }
  else {
     printf("Hello I am in parent process %d with child %d\n",
             getpid(), pid);
     }

  if ((pid= wait(&status)) == -1 ){
     perror("wait failed");
     exit(2);
     }

  if ( (exit_status = WIFEXITED(status)) ) {
     printf("exit status from %d was %d\n",pid, exit_status);
     }

exit(0);
}
