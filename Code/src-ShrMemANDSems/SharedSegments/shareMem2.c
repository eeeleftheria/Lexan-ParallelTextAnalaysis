#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char **argv) {
	int id, err;
	int *mem;

	if (argc <= 1) { 
		printf("Need shared memory id. \n"); exit(1); }

	/* Get id from command line. */
	sscanf(argv[1], "%d", &id);
	printf(">> The Shared Segment Id is: %d\n", id);

	/* Attach the segment */
	mem = (int *) shmat(id, (void*) 0,0);
	if (*(int *) mem == -1) perror("Attachment.");
	else printf(">> Attached Shared Segment whose Mem content is: %d\n",*mem);

	/* Give it a different value */
	*mem=2;
	printf(">> Changed Shared Segment ; Memory is now: %d\n", *mem);

	/* Detach segment */
	err = shmdt((void *) mem);
	if (err == -1) perror ("Detachment.");
	else printf(">> Detachment of Shared Segment %d\n", err);
	return 0;
}


