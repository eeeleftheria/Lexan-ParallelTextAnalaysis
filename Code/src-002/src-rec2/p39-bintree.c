#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
   int i, depth, numb, pid1, pid2, status; 

   if (argc >1)  depth = atoi(argv[1]);
   else { printf("Usage: %s DepthNum\n", argv[0]); exit(0);}

   if (depth>5) {
      printf("Depth should be up to 5\n");
      exit(0);
      }

   numb = 1;
   for(i=0;i<depth;i++){
      printf("I am process no %d  with PID %d and PPID %d\n",
              			numb, getpid(), getppid());
      switch (pid1=fork()){
      case 0:
         	numb=2*numb; break;
      case -1:
         	perror("fork"); exit(1);
      default:
         	switch (pid2=fork()){
            	case 0:
               		numb=2*numb+1; break;
            	case -1:
               		perror("fork"); exit(1);
            	default:
			/* double wait here - whoever comes in first! */
               		wait(&status); 
			wait(&status);
               	exit(0);
            	}
     }
   }
}
