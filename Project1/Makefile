# Compiler
CC = gcc

# Directories
SRC_DIR = Src
INC_DIR = Include
OBJ_DIR = Obj

# Flags
CFLAGS = -Wall -g -I$(INC_DIR)

# Files
SRCS = $(SRC_DIR)/graph.c \
       $(SRC_DIR)/list.c \
       $(SRC_DIR)/miris.c \
       $(SRC_DIR)/hash.c

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

# Clean
clean:
	rm -rf $(OBJ_DIR) $(EXEC)

# Run
run: $(EXEC)
	./$(EXEC) 
# Valgrind
valgrind: $(EXEC)
	valgrind --leak-check=full ./$(EXEC)