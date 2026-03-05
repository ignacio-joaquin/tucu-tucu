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

clean:
	rm -f main sim robot