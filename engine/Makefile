#include .env

CFLAGS = -std=c++17 -I.
LDFLAGS = `pkg-config --static --libs glfw3` -lvulkan

GLSLC = glslc

# create list of all spv files and set as dependency
vertSources = $(shell find ./shaders -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertSources))
fragSources = $(shell find ./shaders -type f -name "*.frag")
fragObjFiles = $(patsubst %.frag, %.frag.spv, $(fragSources))

# create list of all cpp files and set as dependency
cppSources = $(wildcard *.cpp)
cppHeaders = $(wildcard *.hpp)
cppObjFiles = $(patsubst %.cpp, build/%.o, $(cppSources))

TARGET = pong.out
$(TARGET): $(vertObjFiles) $(fragObjFiles) $(cppObjFiles)
	g++ $(cppObjFiles) -o $(TARGET) $(LDFLAGS)

# make shader targets
%.spv: %
	${GLSLC} $< -o $@

# make object file targets
build/%.o: %.cpp $(cppHeaders)
	@mkdir -p build
	g++ $(CFLAGS) -c $< -o $@

.PHONY: test clean

test: a.out
	./a.out

clean:
	rm -f a.out
	rm -f shaders/*.spv
	rm -f build/*.o
