#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]){
   int i, depth, numb, pid1, pid2, status; 

   if (argc >1)  depth = atoi(argv[1]);
   else { printf("Usage: %s #-of-Params",argv[0]); exit(0);}

   if (depth>5) {
      printf("Depth should be up to 5\n");
      exit(0);
      }

   numb = 1;
   for(i=0;i<depth;i++){
      printf("I am process no %5d  with PID %5d and PPID %d\n",
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
               wait(&status); wait(&status);
               exit(0);
            }
     }
   }
}
