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

Controller::Controller(int nSwitch, int listen_port) :
    NetworkNode(0, nSwitch),
    switches_((size_t) nSwitch)
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    listen_fd_ = { lfd, POLLIN, 0 };
    bind(listen_port);
    listen();
}

Controller::~Controller() {
    close(listen_fd_.fd);
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

void Controller::loop() {
    accept();

    // Poll unknown connections
    if (poll(unknown_conns_.data(), unknown_conns_.size(), 0)) {
        for (size_t i = 0; i < unknown_conns_.size(); i++) {
            if (unknown_conns_[i].revents & POLLIN) {
                // For unknown connections, we need to receive an open connection
                // before processing any other packets
                unique_ptr<Packet> packet = unknown_ports_.at(i)->readPacket();

                if (packet && packet->type == OPEN) {
                    // Register switch
                    handleOpenPacket((int) i, dynamic_cast<const OpenPacket*>(packet.get()));
                }
            }
        }
    }

    NetworkNode::loop();
}

string Controller::getType() const
{
    return "controller";
}

void Controller::processPacket(int port, const unique_ptr<Packet>& packet) {
    switch (packet->type) {
        // The controller shouldn't receive OPEN packets through this callback
        case OPEN:
            std::cout << "Error: received OPEN packet, but did not process" << std::endl;
            break;
        case QUERY:
            handleQueryPacket(port, packet.get());
            query_count_++;
            add_count_++;
            break;
        case CLOSE:
            closePort(port);
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
    open_count_++;
    switches_.at((size_t) (op->sw - 1)) = SwitchInfo(op->sw, op->left, op->right, op->ipLow, op->ipHigh);
    printf("\rReceived %s\n", op->toString(op->sw, getId()).c_str());

    // Move unknown port to known port
    unknown_ports_.at((size_t) port)->setDst(op->sw);
    setPort(op->sw - 1, unknown_ports_.at((size_t) port));
    removeUnknownConnection(port);

    Packet ack(PacketType::ACK, getId(), port + 1);
    transmitPacket(port, ack);
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

void Controller::bind(int listen_port) {
    sockaddr_in serv;
    memset((char *)&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr = { .s_addr = htonl(INADDR_ANY) };
    serv.sin_port = static_cast<unsigned short>(listen_port);

    if (::bind(listen_fd_.fd, (sockaddr *)&serv, sizeof(serv)) < 0) {
        perror("Error on bind");
        fflush(stdout);
    }
}

void Controller::listen() {
    if (::listen(listen_fd_.fd, MAX_NSW) < 0) {
        perror("Error on listen");
        fflush(stdout);
    }
}

void Controller::accept() {
    if (poll(&listen_fd_, 1, 0)) {
        if (listen_fd_.revents & POLLIN) {
            int conn_fd;

            if ((conn_fd = ::accept(listen_fd_.fd, nullptr, nullptr)) < 0) {
                perror("Error on accept");
                fflush(stdout);
                return;
            }

            addUnknownConnection(conn_fd);
        }
    }
}

void Controller::addUnknownConnection(int sfd) {
    shared_ptr<Port> port(new SocketPort(sfd));
    unknown_conns_.emplace_back(pollfd { port->rfd(), POLLIN, 0 });
    unknown_ports_.emplace_back(std::move(port));
}

void Controller::removeUnknownConnection(int port) {
    if (unknown_ports_.size() > 1) {
        std::iter_swap(unknown_ports_.begin() + port, unknown_ports_.end() - 1);
        std::iter_swap(unknown_conns_.begin() + port, unknown_conns_.end() - 1);

        unknown_ports_.pop_back();
        unknown_conns_.pop_back();
    } else {
        unknown_ports_.clear();
        unknown_conns_.clear();
    }
}
