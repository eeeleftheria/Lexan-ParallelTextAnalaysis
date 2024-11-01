//############################# SYSTEM CALLS ########################//
// exec*(), mkfifo(), pipe(), open(), close(), read(), write(),      //
// wait(), waitpid(), poll(), select(), dup(), dup2(), kill(), exit()//
//###################################################################//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

# define READ 0
# define WRITE 1
# define BUFSIZE 100


char* mystring = " This is a test only ";
int main ( void ){
    pid_t pid ;
    int fd [2];
    int bytes;
    char message [BUFSIZE];

    if ( pipe ( fd ) == -1){ 
        perror ( " pipe error" ); 
        exit (1); 
    }
    if ( (pid = fork ()) == -1 ){ 
        perror (" fork error" );
        exit (1); 
    }
    if ( pid == 0 ){ // child
        close ( fd [ READ ]); //fd[0] πρεπει να το κλεισει γιατι το παιδι θελει μονο να γραψει
        write ( fd [ WRITE ], mystring , strlen ( mystring )+1);
        close ( fd [ WRITE ]); //τελειωσε με το γραψιμο
    }
    else { // parent
        close ( fd [ WRITE ]);
        bytes = read ( fd [ READ ], message, sizeof (message));
        printf ( " Read %d bytes :%s \n", bytes, message);
        close ( fd [ READ ]);
    }
}