#define compiler and flags.
CXX=g++
CXXFLAGS=-Iinclude -Wall -g

#define source files and object files.
SRCS=$(wildcard src/*.cpp)
OBJS=$(SRCS:src/%.o=build/%.o)

#target
TARGET=riscv_sim

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

build/%.o : src/%.cpp
	@mkdir build
	$(CXX) $(CXXFLAGS) $< -o $@ -c

clean:
	rm -r build