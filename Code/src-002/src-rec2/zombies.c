#include <stdlib.h>
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
		sleep(500); 
		}
      }
   else { 
      exit(37);
      }
}
