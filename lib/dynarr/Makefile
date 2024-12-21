EXEC := main
CC := gcc
SRC := $(shell find . -maxdepth 5 -name "*.c" -not -path "./test/*")
INCLUDE_DIRS := $(shell find ../ -maxdepth 2 -type d -name "include")
CFLAGS := $(addprefix -I, $(INCLUDE_DIRS))

OBJ := $(patsubst %.c, %.o, $(SRC))

# Default target
$(EXEC): $(OBJ)
	$(CC) -c $(OBJ) $(LDFLAGS) $(LDLIBS)

# Compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJ) $(EXEC)