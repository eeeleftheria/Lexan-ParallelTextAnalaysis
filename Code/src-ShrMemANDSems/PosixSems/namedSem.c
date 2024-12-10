#include 	<stdio.h>
#include 	<stdlib.h>
#include	<string.h>
#include 	<assert.h>
#include 	<string.h>
#include	<errno.h>
#include	<unistd.h>
#include	<sys/file.h>
#include	<sys/stat.h>
#include	<semaphore.h>

int main(int argc, char *argv[]){

const char *semname;
int op = 0; int val=0;

if (argc==3)  {
	semname=argv[1];
	op=atoi(argv[2]);
	}
else	{ 
	printf("usage: nameSem nameOfSem Operation\n"); 
	exit(1);
	}
	

sem_t *sem=sem_open(semname, O_CREAT|O_EXCL, S_IRUSR|S_IWUSR, 0);

if (sem!= SEM_FAILED)
	printf("created new semaphore!\n");
else if (errno== EEXIST ) {
	printf("semaphore appears to exist already!\n");
	sem = sem_open(semname, 0);
	}
else ;

assert(sem != SEM_FAILED);

sem_getvalue(sem, &val);
printf("semaphore's before action value is %d\n",val);

if ( op == 1 ) { 
	printf("incrementing semaphore\n"); 
	sem_post(sem);
	}
else if ( op == -1 ) { 
	printf("decrementing semaphore\n"); 
	sem_wait(sem);
	}
else if ( op == 2 ){
	printf("clearing up named semaphore\n");
	sem_close(sem);  // close the sem
	sem_unlink(semname); // remove it from system
	exit(1);
	}
else 	printf("not defined operation! \n");

sem_getvalue(sem, &val);
printf("semaphore's current value is %d\n",val);
sem_close(sem);
return(0);
}
