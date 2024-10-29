#include	<stdio.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<string.h>
#include        <stdlib.h>

int	main(void){
        pid_t childpid;


	printf("Hello 1 \n");

        childpid=fork();

        if (childpid== -1){
                perror("Failed to fork\n");
                exit(1);
                }

	printf("Hello 2 \n");

        return(0);
}
