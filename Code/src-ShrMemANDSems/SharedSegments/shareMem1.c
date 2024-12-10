#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char **argv){
	int id=0, err=0;
	int *mem;

	/* Make shared memory segment */
	id = shmget(IPC_PRIVATE,10,0666); 
	if (id == -1) 
		perror ("Creation");
	else 	
		printf("Allocated Shared Memory with ID: %d\n",(int)id);

	/* Attach the segment */
	mem = (int *) shmat(id, (void*)0, 0);
	if ( *(int *) mem == -1) 
		perror("Attachment.");
	else 
		printf("Just Attached Shared Memory whose content is: %d\n",*mem);

	/* Give it initial value */
	*mem=1;
	printf("Just Altered Shared Memory content to: %d\n",*mem);

	printf("Start other process. >"); getchar();

	/* Print out new value */
	printf("Content of Shared Mem is now: %d\n", *mem);

	/* Remove segment */
	err = shmctl(id, IPC_RMID, 0);
	if (err == -1) 
		perror ("Removal.");
	else 
		printf("Just Removed Shared Segment. %d\n", (int)(err));
	return 0;
}


