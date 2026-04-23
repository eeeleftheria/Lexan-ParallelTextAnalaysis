# Compiler
CC = gcc

# Directories
SRC_DIR = Src
INC_DIR = Include
OBJ_DIR = Obj

# Flags
CFLAGS = -Wall -g -I$(INC_DIR)

# Files
SRCS = $(SRC_DIR)/graph.c $(SRC_DIR)/list.c $(SRC_DIR)/miris.c $(SRC_DIR)/hash.c

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

EXEC = miris

# Default target
all: $(EXEC)

# Link
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

# Compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create obj directory if it not exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

run1: 
	valgrind ./miris -i Input/data8-3.txt -o output.txt

run2: 
	valgrind ./miris -i Input/data15-3.txt -o output.txt

run3: 
	valgrind ./miris -i Input/data100-4.txt -o output.txt

run4: 
	valgrind ./miris -i Input/data100-8.txt -o output.txt

run5: 
	valgrind ./miris -i Input/data1000-25.txt -o output.txt

# Clean
clean:
	rm -rf $(OBJ_DIR) $(EXEC)

# Valgrind
valgrind: $(EXEC)
	valgrind --leak-check=full ./$(EXEC)