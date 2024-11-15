#compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall  -g -I ./Include

# Αρχεία .o
OBJ_ROOT =  root.o
OBJ_SPLIT = splitter.o list.o hash.o
OBJ_BUILD = builder.o list.o hash.o
OBJ_HASH = hash.o list.o
OBJ_LIST = list.o

# Το εκτελέσιμο πρόγραμμα
EXEC_ROOT = lexan
EXEC_SPLIT = splitter
EXEC_BUILD = builder
EXEC_HASH = hash
EXEC_LIST = list

all: $(EXEC_ROOT) $(EXEC_SPLIT) $(EXEC_BUILD) $(EXEC_LEXAN)

$(EXEC_HASH): $(OBJ_HASH)
	$(CC) $(OBJ_HASH) -o $(EXEC_HASH) $(LDFLAGS)

$(EXEC_LIST): $(OBJ_LIST)
	$(CC) $(OBJ_LIST) -o $(EXEC_LIST) $(LDFLAGS)

$(EXEC_ROOT): $(OBJ_ROOT)
	$(CC) $(OBJ_ROOT) -o $(EXEC_ROOT) $(LDFLAGS)

$(EXEC_SPLIT): $(OBJ_SPLIT)
	$(CC) $(OBJ_SPLIT) -o $(EXEC_SPLIT) $(LDFLAGS)

$(EXEC_BUILD): $(OBJ_BUILD)
	$(CC) $(OBJ_BUILD) -o $(EXEC_BUILD) $(LDFLAGS)

clean:
	rm -f $(OBJ_ROOT) $(OBJ_SPLIT) $(OBJ_BUILD) $(EXEC_ROOT) $(EXEC_SPLIT) $(EXEC_BUILD) $(OBJ_HASH) $(EXEC_HASH) $(OBJ_LIST) $(EXEC_LIST)
