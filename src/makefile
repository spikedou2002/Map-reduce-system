CXX = g++

all: server client

server: test.cpp Coordinator.cpp
	$(CXX) -o server test.cpp Coordinator.cpp

client: test_c.cpp Worker.cpp
	$(CXX) -o client test_c.cpp Worker.cpp

.PHONY: clean
clean:
	rm -rf server client
