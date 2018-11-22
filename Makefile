CXX_FLAGS = -std=c++11 -Wall

all: switch.o controller.o src/a3sdn.cpp
	g++ $(CXX_FLAGS) src/a3sdn.cpp bin/*.o -Iinclude -o a3sdn

switch.o: network_node.o include/switch.h src/switch.cpp
	g++ $(CXX_FLAGS) -c src/switch.cpp -Iinclude -o bin/switch.o

controller.o: network_node.o include/controller.h src/controller.cpp
	g++ $(CXX_FLAGS) -c src/controller.cpp -Iinclude -o bin/controller.o

network_node.o: flow_rule.o packet.o port.o include/network_node.h src/network_node.cpp
	g++ $(CXX_FLAGS) -c src/network_node.cpp -Iinclude -o bin/network_node.o

port.o: packet.o include/port.h src/port.cpp
	g++ $(CXX_FLAGS) -c src/port.cpp -Iinclude -o bin/fifo.o

packet.o: include/packet.h src/packet.cpp
	g++ $(CXX_FLAGS) -c src/packet.cpp -Iinclude -o bin/packet.o

flow_rule.o: include/flow_rule.h src/flow_rule.cpp
	g++ ${CXX_FLAGS} -c src/flow_rule.cpp -Iinclude -o bin/flow_rule.o

clean:
	rm -f bin/*.o fifo-* a3sdn

tar:
	find . -maxdepth 2 -type f \( -name '*.cpp' -o -name '*.h' -o -name '.placeholder' \) | sed -e 's/\.\///g' | \
        tar -cvf submit.tar --files-from - docs/a3sdn-design-document.pdf Makefile
