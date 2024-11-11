#include	<stdio.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<string.h>
#include        <stdlib.h>

int	main(void){
        pid_t childpid;

        childpid=fork();

        if (childpid== -1){
                perror("Failed to fork\n");
                exit(1);
                }

        if (childpid==0)
		printf("I am the child process  with ID: %lu \n",(long)getpid());
        else    printf("I am the parent process with ID: %lu \n",(long)getpid());
        return(0);
}
