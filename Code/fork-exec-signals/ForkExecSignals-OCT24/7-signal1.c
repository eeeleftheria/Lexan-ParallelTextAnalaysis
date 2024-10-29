#include <stdio.h>
#include <signal.h>
#include <unistd.h>


void f(int), f1(int);

int main (){
	int i;

	signal(SIGINT , f); // ???

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

