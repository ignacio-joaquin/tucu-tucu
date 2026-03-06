CXX      = g++
CXXFLAGS = -std=c++17 -Wall -I$(SRC)
LDFLAGS  = -lrt
SRC      = src

all: main sim robot

main: $(SRC)/main.cpp $(SRC)/shared/ipc_layout.h
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)/main.cpp $(LDFLAGS)


sim: $(SRC)/sim.cpp $(SRC)/shared/ipc_layout.h
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)/sim.cpp $(LDFLAGS)


robot: $(SRC)/robot.cpp $(SRC)/shared/ipc_layout.h
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)/robot.cpp $(LDFLAGS)

test-mazIO: tests/mazio_test.c src/lib/mazIO/maze.c src/lib/mazIO/stack.c src/lib/mazIO/queue.c src/lib/mazIO/import.c
	gcc -std=c11 -Wall -I$(SRC) -o $@ tests/mazio_test.c src/lib/mazIO/maze.c src/lib/mazIO/stack.c src/lib/mazIO/queue.c src/lib/mazIO/import.c

clean:
	rm -f main sim robot test-mazIO