#compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall  -g -I ./Include

# Αρχεία .o
OBJ_ROOT =  root.o
OBJ_SPLIT = splitter.o
OBJ_BUILD = builder.o

# Το εκτελέσιμο πρόγραμμα
EXEC_ROOT = lexan
EXEC_SPLIT = splitter
EXEC_BUILD = builder

all: $(EXEC_ROOT) $(EXEC_SPLIT) $(EXEC_BUILD) $(EXEC_LEXAN)

#$(EXEC): $(OBJS)
#	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

$(EXEC_ROOT): $(OBJ_ROOT)
	$(CC) $(OBJ_ROOT) -o $(EXEC_ROOT) $(LDFLAGS)

$(EXEC_SPLIT): $(OBJ_SPLIT)
	$(CC) $(OBJ_SPLIT) -o $(EXEC_SPLIT) $(LDFLAGS)

$(EXEC_BUILD): $(OBJ_BUILD)
	$(CC) $(OBJ_BUILD) -o $(EXEC_BUILD) $(LDFLAGS)

clean:
	rm -f $(OBJ_ROOT) $(OBJ_SPLIT) $(OBJ_BUILD) $(EXEC_ROOT) $(EXEC_SPLIT) $(EXEC_BUILD)

run: $(EXEC)
	./$(EXEC)
