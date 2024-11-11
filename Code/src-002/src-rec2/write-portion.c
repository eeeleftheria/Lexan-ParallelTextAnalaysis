#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define READ 0
#define WRITE 1

main(int argc, char *argv[] ) {
	char message[]="Hello there!";
	
	// although the program is NOT aware of the logical names 
	// can access/manipulate the file descriptors!!!

	printf("Operating after the execlp invocation! \n");
	if ( write(3,message, strlen(message)+1)== -1)
		perror("Write to 3-file \n");
	else	printf("Write to file with file descriptor 3 succeeded\n");
	
	if ( write(5, message, strlen(message)+1) == -1)
		perror("Write to 5-pipe");
	else	printf("Write to pipe with file descriptor 5 succeeded\n");

	if ( write(7, message, strlen(message)+1) == -1)
		perror("Write to 7-pipe");
	else	printf("Write to pipe with file descriptor 7 succeeded\n");

	if ( write(11, message, strlen(message)+1) == -1)
                perror("Write to 11-dup2");
        else    printf("Write to dup2ed file descriptor 11 succeeded\n");

	if ( write(13, message, strlen(message)+1) == -1)
                perror("Write to 13-invalid");
        else    printf("Write to invalid file descriptor 13 not feasible\n");
	return 1;
}
