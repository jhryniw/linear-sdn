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
    }
}

void Controller::list() {
    cout << "\nSwitch information:\n";

    for(const auto& sw : switches_) {
        if(sw.id == -1) continue;
        printf("[sw%d] port1= %d, port2= %d, port3= %d-%d\n", sw.id, sw.left, sw.right, sw.lowIP, sw.highIP);
    }

    cout << "\nPacket Stats:\n";
    printf("Received:    OPEN:%d, QUERY:%d\n", open_count_, query_count_);
    printf("Transmitted: ACK:%d, ADD:%d\n", ack_count_, add_count_);
    cout << endl;
}

string Controller::getType() const
{
    return "controller";
}

void Controller::processPacket(int port, const unique_ptr<Packet>& packet) {
    switch (packet->type) {
        case OPEN:
            // Register switch
            handleOpenPacket(port, dynamic_cast<const OpenPacket*>(packet.get()));
            break;
        case QUERY:
            handleQueryPacket(port, packet.get());
            query_count_++;
            add_count_++;
            break;
        // The controller shouldn't receive these packet types, ignore them
        case RELAY:
        case ADD:
        case ACK:
        case ADMIT:
        case UNKNOWN:
            break;
    }
}

void Controller::handleOpenPacket(int port, const OpenPacket* op)
{
    Packet resp(PacketType::ACK, getId(), port + 1);
    open_count_++;
    switches_.at((size_t) port) = SwitchInfo(op->sw, op->left, op->right, op->ipLow, op->ipHigh);
    getPort(port)->writePacket(resp);
    ack_count_++;
}

void Controller::handleQueryPacket(int port, const Packet* qp)
{
    for(const auto& sw : switches_) {
        if (between(qp->dstIP, sw.lowIP, sw.highIP)) {
            int dst_port = sw.id > getPort(port)->dst() ? Switch::RIGHT_PORT : Switch::LEFT_PORT;
            FlowRule rule(0, MAX_IP, sw.lowIP, sw.highIP, Action::FORWARD, dst_port);
            AddPacket ap = AddPacket(getPort(port)->dst(), rule);
            transmitPacket(port, ap);
            return;
        }
    }

    // No switch matches the IP
    FlowRule rule(0, MAX_IP, qp->dstIP, qp->dstIP, Action::DROP, 0);
    AddPacket ap = AddPacket(getPort(port)->dst(), rule);
    transmitPacket(port, ap);
}
