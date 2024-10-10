#compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
#CFLAGS = -Wall  -g -I$(INCLUDE)
# Αρχεία .o


OBJS = graph.o list.o miris.o hash.o

# Το εκτελέσιμο πρόγραμμα
EXEC = miris

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) 

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC) 

valgrind: $(EXEC)
	valgrind ./$(EXEC)