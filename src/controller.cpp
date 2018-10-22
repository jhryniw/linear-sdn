#include <controller.h>

using namespace std;

Controller::Controller(int nSwitch) :
    NetworkNode(0, nSwitch)
{
    for (int port = 0; port < nSwitch; port++) {
        setPort(port, port+1);
        cout << "New port " << port << ": read= " << getPort(port)->rfd() << " write= " << getPort(port)->wfd() << endl;
    }
}

void Controller::list() {
    cout << "Controller information" << endl;
}

void Controller::processPacket(int port, const Packet& packet) {
    Packet resp;

    switch (packet.type) {
        case OPEN:
            resp = Packet(PacketType::ACK, -1, -1);
            getPort(port)->writePacket(resp);
            break;
        case ACK:
            break;
        case QUERY:
        case ADD:
        case RELAY:
        case UNKNOWN:
            break;
    }
}
