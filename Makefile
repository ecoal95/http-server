SOURCES = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard lib/*.cpp) $(wildcard lib/**/*.cpp)
OBJECTS = $(patsubst lib/%.cpp, bin/lib/%.o, $(patsubst src/%.cpp, bin/%.o, $(SOURCES)))
FLAGS = -std=c++11 -O2 -Wall -pthread -Ilib
CXX = g++
all: bin/main
	@echo > /dev/null

bin/main: $(OBJECTS)
	@echo "building: $@"
	$(CXX) $(FLAGS) -o bin/main $(OBJECTS)

bin/%.o: src/%.cpp
	@echo "building: $@"
	@mkdir -p bin/$(*D)
	$(CXX) $(FLAGS) $< -c -o $@
	@echo "built: $@"

bin/lib/%.o: lib/%.cpp
	@echo "building: $@"
	@mkdir -p bin/lib/$(*D)
	$(CXX) $(FLAGS) $< -c -o $@
	@echo "built: $@"

clean:
	rm bin/*.o
	rm bin/**/*.o
