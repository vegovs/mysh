# Directory for include files (mysh.h)
IDIR =../include

# Directory for .o files
ODIR =../obj

# Compiler flags: 
# -g 					Generate debugging information
# -Wall 				Recommended compiler warnings
# -O2 					Recommended optimizations
# -D_POSIX_C_SOURCEI 	Include strdup and kill from POSIX 
CC=gcc -g -O2 -Wall -D_POSIX_C_SOURCE -std=c99 
CFLAGS=-I$(IDIR)

# Dependencies, eller include filer osv
_DEPS = mysh.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# .o Files
_OBJ = mysh.o bi.o mdll.o bm.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

# Compile
mysh: $(OBJ)
		$(CC) -o $@ $^ $(CFLAGS) 

# Compile in debug mode
debug: CFLAGS += -DDEBUG -g
debug: mysh

# Safety 
.PHONY: clean

# Clean .o files
clean:
		rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ 
