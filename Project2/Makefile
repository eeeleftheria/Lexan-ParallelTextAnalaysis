CC = gcc

SRC_DIR = Src
INC_DIR = Include
OBJ_DIR = Obj

CFLAGS = -Wall -g -I $(INC_DIR)

# Object files
OBJ_ROOT = $(OBJ_DIR)/root.o
OBJ_SPLIT = $(OBJ_DIR)/splitter.o $(OBJ_DIR)/list.o $(OBJ_DIR)/hash.o
OBJ_BUILD = $(OBJ_DIR)/builder.o $(OBJ_DIR)/list.o $(OBJ_DIR)/hash.o

# Executables
EXEC_ROOT = lexan
EXEC_SPLIT = splitter
EXEC_BUILD = builder

all: $(EXEC_ROOT) $(EXEC_SPLIT) $(EXEC_BUILD) 

# Linking
$(EXEC_ROOT): $(OBJ_ROOT)
	$(CC) $^ -o $@

$(EXEC_SPLIT): $(OBJ_SPLIT)
	$(CC) $^ -o $@

$(EXEC_BUILD): $(OBJ_BUILD)
	$(CC) $^ -o $@

# Compile rule
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create obj dir
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

run1: 
	./lexan -i Testing/Republic_a.txt -l 80 -m 20 -t 20 -e Testing/ExclusionList1_a.txt -o output.txt

run2:
	./lexan -i Testing/GreatExpectations_a.txt -l 80 -m 20 -t 20 -e Testing/ExclusionList1_a.txt -o output.txt

run3:
	./lexan -i Testing/WilliamShakespeareWorks_a.txt -l 80 -m 20 -t 20 -e Testing/ExclusionList2_a.txt -o output.txt

clean:
	rm -rf $(OBJ_DIR) $(EXEC_ROOT) $(EXEC_SPLIT) $(EXEC_BUILD) $(EXEC_HASH) $(EXEC_LIST)