# Compiler
CC = gcc

# Compile options. The -I<dir> tells the compiler to look for include files there
CFLAGS = -Wall -g -I ./Include
LDFLAGS = -lrt -lpthread # Linker flags

# Object files
OBJ_INIT = ./Source/initialization.o
OBJ_MONITOR = ./Source/monitor.o
OBJ_RECEPTIONIST = ./Source/receptionist.o
OBJ_VISITOR = ./Source/visitor.o

# Executable programs
EXEC_INIT = initialization
EXEC_MONITOR = monitor
EXEC_RECEPTIONIST = receptionist
EXEC_VISITOR = visitor

all: $(EXEC_INIT) $(EXEC_MONITOR) $(EXEC_RECEPTIONIST) $(EXEC_VISITOR)

# Link the object files to create executables
$(EXEC_INIT): $(OBJ_INIT)
	$(CC) $(OBJ_INIT) -o $(EXEC_INIT) $(LDFLAGS) $(CFLAGS)

$(EXEC_MONITOR): $(OBJ_MONITOR)
	$(CC) $(OBJ_MONITOR) -o $(EXEC_MONITOR) $(LDFLAGS) $(CFLAGS)

$(EXEC_RECEPTIONIST): $(OBJ_RECEPTIONIST)
	$(CC) $(OBJ_RECEPTIONIST) -o $(EXEC_RECEPTIONIST) $(LDFLAGS) $(CFLAGS)

$(EXEC_VISITOR): $(OBJ_VISITOR)
	$(CC) $(OBJ_VISITOR) -o $(EXEC_VISITOR) $(LDFLAGS) $(CFLAGS)


# Clean up object files and executables
clean:
	rm -f $(OBJ_RECEPTIONIST) $(OBJ_VISITOR) $(OBJ_MONITOR) $(EXEC_RECEPTIONIST) $(EXEC_VISITOR) $(EXEC_MONITOR) $(OBJ_INIT) $(EXEC_INIT)
