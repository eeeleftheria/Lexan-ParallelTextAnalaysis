#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


#define BUFFER_SIZE 10

int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        fprintf(stderr, "instruction is: cp inputfile outputfile\n");
        return -1;
    }

    //διαβασμα μονο
    int inp_file = open(argv[1], O_RDONLY);
    
    if (inp_file < 0) { //αποτυχια ανοιγματος αρχειου
    
        fprintf(stderr, "Failure opening file\n");
        return -1;
    }

    //εγγραφη μονο / δημιουργια αν δεν υπαρχει / διαγραφη περιεχομενων αν υπαρχουν ηδη / δικαιωματα
    int out_file = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    char* buffer[BUFFER_SIZE];

    //αριθμος bytes που διαβαζουμε /γραφουμε
    size_t bytes_to_read, bytes_to_write;

    //Διαβάζουμε κάθε φορά στον buffer και γράφουμε στο outpute file (2 syscalls)
    while ((bytes_to_read = read(inp_file, buffer, BUFFER_SIZE)) > 0) {
        bytes_to_write = write(out_file, buffer, bytes_to_read);
    }

    close(inp_file);
    close(out_file);
    return 0;
}