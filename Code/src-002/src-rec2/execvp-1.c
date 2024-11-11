#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(void){
   int pid, status; 
   char *buff[2];

   if ( (pid=fork()) == -1){
      perror("fork"); exit(1);
      }

   if ( pid!=0 ) { // parent
      printf("I am the parent process %d\n",getpid());
      if (wait(&status) != pid){  //check if child returns
          perror("wait");
          exit(1);
          }
      printf("Child terminated with exit code %d\n", status >> 8);
      }
   else {
      buff[0]=(char *)malloc(12);
      strcpy(buff[0],"date");
      printf("%s\n",buff[0]);
      buff[1]=NULL;

      printf("I am the child process %d ",getpid());
      printf("and will be replaced with 'date'\n");
      //execvp("date",buff);
      execvp("/bin/date",buff);
      exit(1);
      }
}
