#include <stdio.h>
#include <signal.h>
#include <unistd.h>


void f(int);

int main (){
	int i;

	//καθε φορα που δεχεται ενα τετοιο signal που στη συγκεκριμενη περιπτωση ειναι ενα 
	//interrupt, θα εκτελει τον κωδικα της f
	signal(SIGINT , f); // οριζει τον handler του σηματος SIGINT

	for (i=0; i <5;i ++){
		printf(" hello \n");
		sleep (1) ;
		}
	}

void f( int signum ){ /* no explicit call to function f in main()*/
	signal (SIGINT , f); 
	/* re - establish disposition of the signal SIGINT */

	printf ("OUCH !\n");
	}

