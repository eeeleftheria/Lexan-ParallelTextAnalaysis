#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define MSGSIZE 16

char *msg1="Buenos Dias! #1";
char *msg2="Buenos Dias! #2";
char *msg3="Buenos Dias! #3";

main(){
  char inbuf[MSGSIZE];
  int p[2], i=0, rsize=0;
  pid_t pid;

  if (pipe(p)==-1) { perror("pipe call"); exit(1);}

  switch(pid=fork()){
  case -1: perror("fork call"); exit(2);
  case  0: write(p[1],msg1,MSGSIZE);   // if child then write!
           write(p[1],msg2,MSGSIZE);
           write(p[1],msg3,MSGSIZE);
	   break;
  default: for (i=0;i<3;i++){          // if parent then read!
 		rsize=read(p[0],inbuf,MSGSIZE);
		printf("%.*s\n",rsize,inbuf);
		}
	    wait(NULL);
  }
  exit(0);
}

