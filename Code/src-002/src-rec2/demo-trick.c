#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SENTINEL -1

main(){
	pid_t pid; 
	int fd=SENTINEL;

	printf("About to run who into a file (in a strange way!)\n");

	if ( (pid=fork())== SENTINEL){
		perror("fork"); exit(1);
		}

	if ( pid == 0 ){
		close(1);
		fd=creat("userlist", 0644);
		execlp("who","who",NULL);
		perror("execlp");
		exit(1);
		}
	
	if ( pid != 0){
		wait(NULL);
		printf("Done running who - results in file \"userlist\"\n");
		}
}
