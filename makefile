CXX = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall
LINKING = -lglut -lGL -lGLU
TARGET = *
EXE = trabalhocg

all:
	$(CXX) -o $(EXE) $(TARGET).cpp $(LINKING)

clean:
	$(RM) $(TARGET).o $(EXE)
