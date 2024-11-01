#include <stdio.h>
#include <unistd.h>

main(){
  int retval=0;

  printf("I am process %lu and I will execute an 'ls -l .; \n",(long)getpid);

  retval=execl("/bin/ls", "ls", "-l", ".", NULL);

  if (retval==-1)
	perror("execl");
}
