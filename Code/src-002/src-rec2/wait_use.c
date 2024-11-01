#include <stdio.h>
#include <stdlib.h>

int main(){
  pid_t pid;
  int status;

  printf("Original Process: PID = %d\n",getpid());
  pid = fork();
  if (pid == -1 ) {
      perror("fork failed");
      exit(1);
      }
  
   if ( pid!=0 ) {
      printf("Parent process: PID = %d \n",getpid());
      if ( (wait(&status) != pid ) ) {
           perror("wait");
           exit(1);
           }
      printf("Child terminated: PID = %d, exit code = %d\n",pid, status >> 8);
      }
    else {
       printf("Child process: PID = %d, PPID = %d \n", getpid(), getppid());
       exit(62);
       } 
    printf("Process with PID = %d terminates",getpid());
    sleep(1);
}
