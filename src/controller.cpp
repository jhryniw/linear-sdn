#include <controller.h>

using namespace std;

Controller::Controller(int nSwitch) :
    NetworkNode(0, nSwitch)
{
    for (int port = 0; port < nSwitch; port++) {
        setPort(port, port+1);
        cout << "New port " << port << ": read= " << getPort(port).rfd << " write= " << getPort(port).wfd << endl;
    }
}

void Controller::list() {
    cout << "Controller information" << endl;
}

void Controller::exit() {
    list();
}

void Controller::processPacket(const Packet& packet) {
    cout << packet.encode() << endl;
}
