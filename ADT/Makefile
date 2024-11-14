#compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall  -g -I./Include
# Αρχεία .o

OBJS = main.o list.o hash.o

# Το εκτελέσιμο πρόγραμμα
EXEC = main

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC)

valgrind: $(EXEC)
	 valgrind --leak-check=full ./$(EXEC)