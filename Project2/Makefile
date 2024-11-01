#compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall  -g -I ./Include
# Αρχεία .o

OBJS = lexan.o

# Το εκτελέσιμο πρόγραμμα
EXEC = lexan

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC)

valgrind: $(EXEC)
	./$(EXEC)