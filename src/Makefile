CC = g++
OPT = -O3
#OPT = -g
WARN = -Wall
CFLAGS = $(OPT) $(WARN) $(INC) $(LIB)

# List all your .cc files here (source files, excluding header files)
SIM_SRC = main.cpp cpu.cpp common.cpp

# List corresponding compiled object files here (.o files)
SIM_OBJ = main.o cpu.o common.o
 
#################################

# default rule

all: ooosim
	@echo "Done with make ..."


# rule for making ooosim
ooosim: $(SIM_OBJ)
	$(CC) -o ooosim $(CFLAGS) $(SIM_OBJ) -lm

# generic rule for converting any .cc file to any .o file
%.o:%.cpp
	$(CC) $(CFLAGS) -I./ -c $^ 


# type "make clean" to remove all .o files plus the ooosim binary
clean:
	rm -f *.o ooosim

# type "make clobber" to remove all .o files (leaves ooosim binary)
clobber:
	rm -f *.o


