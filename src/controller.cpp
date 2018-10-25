#include <controller.h>

using namespace std;

SwitchInfo::SwitchInfo() :
    id(-1),
    left(-1),
    right(-1),
    lowIP(-1),
    highIP(-1)
{

}

SwitchInfo::SwitchInfo(int id, int left, int right, int low_ip, int high_ip) :
    id(id),
    left(left),
    right(right),
    lowIP(low_ip),
    highIP(high_ip)
{

}

Controller::Controller(int nSwitch) :
    NetworkNode(0, nSwitch),
    switches_((size_t) nSwitch)
{
    for (int port = 0; port < nSwitch; port++) {
        setPort(port, port+1);
        cout << "New port " << port << ": read= " << getPort(port)->rfd() << " write= " << getPort(port)->wfd() << endl;
    }
}

void Controller::list() {
    cout << "\nSwitch information:\n";

    for(const auto& sw : switches_) {
        printf("[sw%d] port1= %d, port2= %d, port3= %d-%d\n", sw.id, sw.left, sw.right, sw.lowIP, sw.highIP);
    }

    cout << "\nPacket Stats:\n";
    printf("Received:    OPEN:%d, QUERY:%d\n", open_count_, query_count_);
    printf("Transmitted: ACK:%d, ADD:%d\n", ack_count_, add_count_);
    cout << endl;
}

void Controller::processPacket(int port, const unique_ptr<Packet>& packet) {
    Packet resp;
    const OpenPacket* op;

    switch (packet->type) {
        case OPEN:
            open_count_++;

            // Register switch
            op = dynamic_cast<const OpenPacket*>(packet.get());
            switches_.at((size_t) port) = SwitchInfo(op->sw, op->left, op->right, op->ipLow, op->ipHigh);
            resp = Packet(PacketType::ACK, -1, -1);
            getPort(port)->writePacket(resp);

            ack_count_++;
            break;
        case ACK:
            break;
        case QUERY:
            query_count_++;
            break;
        case ADD:
            break;
        case RELAY:
            break;
        case UNKNOWN:
            break;
    }
}
