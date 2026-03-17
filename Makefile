CXX      = g++
CXXFLAGS = -std=c++17 -Wall -I$(SRC)
LDFLAGS  = -lrt
SRC      = src

all: main sim robot

main: $(SRC)/main.cpp $(SRC)/shared/ipc_layout.h
	$(CXX) $(CXXFLAGS) -o $@.out $(SRC)/main.cpp $(LDFLAGS)


MAZIO_C_SRCS = src/lib/mazIO/maze.c src/lib/mazIO/stack.c src/lib/mazIO/queue.c
MAZIO_OBJS = $(MAZIO_C_SRCS:.c=.out)

sim: $(SRC)/sim.cpp $(SRC)/shared/ipc_layout.h $(MAZIO_OBJS)
	$(CXX) $(CXXFLAGS) -o $@.out $(SRC)/sim.cpp $(MAZIO_OBJS) $(LDFLAGS)

src/lib/mazIO/%.o: src/lib/mazIO/%.c
	gcc -std=c11 -Wall -I$(SRC) -c $< -o $@


robot: $(SRC)/robot.cpp $(SRC)/shared/ipc_layout.h
	$(CXX) $(CXXFLAGS) -o $@.out $(SRC)/robot.cpp $(LDFLAGS)

test-mazIO: tests/mazio_test.c src/lib/mazIO/maze.c src/lib/mazIO/stack.c src/lib/mazIO/queue.c src/lib/mazIO/import.c
	gcc -std=c11 -Wall -I$(SRC) -o $@.out tests/mazio_test.c src/lib/mazIO/maze.c src/lib/mazIO/stack.c src/lib/mazIO/queue.c src/lib/mazIO/import.c

clean:
	rm -f main sim robot test-mazIO