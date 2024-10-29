/* Example of how 2 processes can talk */
/* to each other using kill() and signal() */
/* We will fork() 2 process and let the parent send a few */
/* signals to it`s child  */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler(); /* routines child will call upon sigtrap */
void sigquit();

int main() {
  int pid;

  /* get child process */

   if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    }

   if (pid == 0)
     { /* child */
       signal(SIGUSR1, handler); /* set function calls */
       signal(SIGQUIT, sigquit);
       printf("\nCHILD: I am about to start looping here..\n");
       for(;;); /* loop for ever */
     }
  else /* parent */
     {  /* pid hold id of child */
       printf("\nPARENT: I am in parent process now...\n");
       sleep(3);
       printf("\nPARENT: sending SIGUSR1\n\n");
       kill(pid,SIGUSR1);
       sleep(3); /* pause for 3 secs */
       printf("\nPARENT: sending SIGQUIT\n\n");
       kill(pid,SIGQUIT);
       sleep(3);
     }
}

void handler() {  // need no param here??
	signal(SIGUSR1,handler);
   	printf("CHILD: I have received a SIGUSR1\n");
	}

void sigquit()
{ printf("CHILD: My DADDY has just Killed me!!!\n");
  exit(0);
}
