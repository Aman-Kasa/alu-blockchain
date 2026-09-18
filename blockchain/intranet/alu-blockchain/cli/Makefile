# Define the compiler
CC = gcc

# Compiler flags: enable all warnings, treat warnings as errors, enable extra warnings, enforce standards compliance
CFLAGS = -Wall -Werror -Wextra -pedantic -I.

# Command to remove files
RM = rm -f

# Command to generate tags for source code files
CTAGS = ctags

# Name of the tags file
TAGS = .tags

# Libraries and paths needed for linking
LIBS = -L../blockchain/v0.3 -lhblk_blockchain -L../crypto -lhblk_crypto -lllist -lssl -lcrypto -pthread

# Source files collected from current directory
SRC = $(wildcard *.c)

# Object files generated from source files
OBJ = $(patsubst %.c,%.o,$(SRC))

# Phony targets for managing commands
.PHONY: all clean distclean oclean tags cli

# Default target: build the CLI application
all: cli

# Remove the compiled application
clean: oclean
	$(RM) cli

# Remove the compiled application and tags file
distclean: clean
	$(RM) $(TAGS)

# Remove object files
oclean:
	$(RM) $(OBJ)

# Generate tags for all relevant source files
tags:
	$(CTAGS) -f $(TAGS) -R

# Link object files into a single executable
cli: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

