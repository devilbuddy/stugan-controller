# List the object files here!
OBJS = controller.o

# Set this to your preferred compiler executable:
CC=gcc

# Include the options here for compiling a c source file to an object file:
CFLAGS=-c -std=gnu99 -Wall

# The main target "all" - this is a very common target in makefiles. Almost all
# makefiles have an all target
all: controller

# Executable link - use the c compiler as the linker
controller: $(OBJS)
	$(CC) $(OBJS) -o controller -ltelldus-core

.c.o:
	$(CC) $(CFLAGS) $<

clean:
	@rm -rf *.o controller *~ 2>/dev/null || true 
