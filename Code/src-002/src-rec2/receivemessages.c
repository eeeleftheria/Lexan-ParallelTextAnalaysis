#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MSGSIZE 65

char *fifo = "myfifo";

main(int argc, char *argv[]){
	int fd, i, nwrite;
	char msgbuf[MSGSIZE+1];

	if (argc>2) { printf("Usage: receivemessage & \n"); exit(1); }

	if ( mkfifo(fifo, 0666) == -1 ){
		if ( errno!=EEXIST ) { perror("receiver: mkfifo"); exit(6); };
		}
	if ( (fd=open(fifo, O_RDWR)) < 0){
		perror("fifo open problem"); exit(3);	
		}
	for (;;){
		if ( read(fd, msgbuf, MSGSIZE+1) < 0) {
			perror("problem in reading"); exit(5);
			}
		fflush(stdout);
		printf("\nMessage Received: %s\n", msgbuf);
		fflush(stdout);
		}
}

