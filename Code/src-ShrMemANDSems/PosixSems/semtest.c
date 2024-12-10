/* semtest.c:  Posix Semaphore test example */

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h> 
#include <sys/types.h>
#include <sys/ipc.h>

extern int errno;

int main(int argc, char **argv)
{
	sem_t sp;
	int retval;

	/* Initialize the semaphore. */
	retval = sem_init(&sp,1,2);
	if (retval != 0) {
		perror("Couldn't initialize.");
		exit(3);
	}
	
	retval = sem_wait(&sp);
	printf("Did trywait. Returned %d >\n",retval); getchar();
	
	retval = sem_trywait(&sp);
	printf("Did trywait. Returned %d >\n",retval); getchar();

	retval = sem_trywait(&sp);
	printf("Did trywait. Returned %d >\n",retval); getchar();

	sem_destroy(&sp);
	return 0;
}

