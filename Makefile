CXX_FLAGS = -std=c++11 -Wall -g

all: switch.o controller.o src/a2sdn.cpp
	g++ $(CXX_FLAGS) src/a2sdn.cpp bin/*.o -Iinclude -o a2sdn

switch.o: network_node.o include/switch.h src/switch.cpp
	g++ $(CXX_FLAGS) -c src/switch.cpp -Iinclude -o bin/switch.o

controller.o: network_node.o include/controller.h src/controller.cpp
	g++ $(CXX_FLAGS) -c src/controller.cpp -Iinclude -o bin/controller.o

network_node.o: packet.o fifo.o include/network_node.h src/network_node.cpp
	g++ $(CXX_FLAGS) -c src/network_node.cpp -Iinclude -o bin/network_node.o

fifo.o: packet.o include/fifo.h src/fifo.cpp
	g++ $(CXX_FLAGS) -c src/fifo.cpp -Iinclude -o bin/fifo.o

packet.o: include/packet.h src/packet.cpp
	g++ $(CXX_FLAGS) -c src/packet.cpp -Iinclude -o bin/packet.o

clean:
	rm -f bin/*.o fifo-* a2sdn
