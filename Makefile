SOURCES = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
OBJECTS = $(patsubst src/%.cpp, bin/%.o, $(SOURCES))
FLAGS = -std=c++11 -O2 -Wall -pthread
CXX = g++
all: bin/main
	@echo > /dev/null

bin/main: $(OBJECTS)
	@echo "building: $@"
	$(CXX) $(FLAGS) -o bin/main $(OBJECTS)

bin/%.o: src/%.cpp
	@echo "building: $@"
	@mkdir -p $(*D)
	$(CXX) $(FLAGS) $< -c -o $@
	@echo "built: $@"

clean:
	rm bin/*.o
	rm bin/**/*.o
