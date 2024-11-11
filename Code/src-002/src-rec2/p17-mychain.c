#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char *argv[]){
pid_t childpid = 0, tt=0;
int i,n;

if (argc!=2){
  fprintf(stderr,"Usage: %s #processes \n",argv[0]);
  return 1;
}

fprintf(stdout,">>> process ID:%ld parent ID:%ld child ID:%ld\n",
	(long)getpid(),(long)getppid(),(long)childpid );

n=atoi(argv[1]);

for(i=1;i<n;i++)
  if ( (childpid = fork()) > 0 ) /* only the child carries on */
    break;

fprintf(stdout,"i:%d process ID:%d parent ID:%d child ID:%d\n",
	i, getpid(), getppid(), childpid );
sleep(1);

return 0;
}

