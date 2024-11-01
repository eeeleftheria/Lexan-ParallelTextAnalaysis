#include <stdio.h>
#include <unistd.h>

int main(){
   printf("Process has as ID the number: %ld \n",(long)getpid()); 
   printf("Parent of the Process has as ID: %ld \n",(long)getppid()); 
   return 0;
}
