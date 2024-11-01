
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

# define READ 0
# define WRITE 1

// αν το τρεξουμε ως ./pipe2 ls wc τα περιεχομενα που επιστρεφει το Ls θα τα παρει το wc
// ως εισοδο και θα εκτυπωσει: αριθμο γραμμων, αριθμο λεξεων, αριθμος χαρακτηρων
int main ( int argc, char* argv[]){
    pid_t pid;
    int fd [2], bytes;

    if (pipe(fd) == -1){ 
        perror( " pipe error" ); 
        exit (1); 
    }
    if((pid = fork ()) == -1 ){ 
        perror (" fork error" ); 
        exit (1); 
    }
    if(pid != 0 ){ // parent and writer
        close (fd[READ]); //θελουμε μονο να γραψουμε
        dup2 (fd [WRITE], 1); //το stdout(1) θα ανακατευθυνθει εκει που δειχνει το fd[WRITE]
        close (fd[WRITE]);
        execlp (argv[1], argv[1], NULL ); // Anything that argv [1] writes goes to the pipe .
        perror ( "execlp error" ); 
    }
    else { // child and reader
        close(fd[WRITE]);
        dup2(fd[READ], 0); //το stdin θα ανακατευθυνθει εκει που δειχνει το fd[read]
        close(fd[READ]);
        execlp(argv[2], argv[2], NULL ); // Anything that argv [2] reads is obtained from the pipe .
    }  
}