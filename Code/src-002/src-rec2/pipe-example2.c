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
  case  0: close(p[0]);                // child is writing
           write(p[1],msg1,MSGSIZE);   
           write(p[1],msg2,MSGSIZE);
           write(p[1],msg3,MSGSIZE);
	   break;
  default: close(p[1]);                // parent is reading
           for (i=0;i<3;i++){          
 		rsize=read(p[0],inbuf,MSGSIZE);
		printf("%.*s\n",rsize,inbuf);
		}
	    wait(NULL);
  }
  exit(0);
}

