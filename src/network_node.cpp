#include <network_node.h>

using namespace std;

const char* YELLOW = "\033[1;33m";
const char* DEFAULT = "\033[0m";

const pollfd NULL_POLL_FD = { -1, 0, 0 };

NetworkNode::NetworkNode(int id, int max_ports) :
    node_ok(true),
    id_(id),
    port_fds_((size_t) max_ports, NULL_POLL_FD),
    ports_((size_t) max_ports),
    prompt_displayed_(false)
{
    stdin_fd_ = pollfd { STDIN_FILENO, POLLIN, 0 };
}

void NetworkNode::loop() {
    // Setup cmd prompt
    if (!prompt_displayed_) {
        cout << YELLOW << "cmd[" << DEFAULT << getType() << YELLOW << "]: " << DEFAULT;
        cout.flush();
        prompt_displayed_ = true;
    }

    // Poll stdin
    if (poll(&stdin_fd_, 1, 0)) {
        if (stdin_fd_.revents & POLLIN) {
            memset(buf_, 0, MAX_BUF);
            read(stdin_fd_.fd, buf_, MAX_BUF);
            string incoming(buf_);

            // Trim trailing characters
            incoming.erase(incoming.find_last_not_of("\t\f\v\n\r\x7F") + 1);

            if (incoming == "list") {
                list();
            } else if (incoming == "exit") {
                exit();
            } else if (!incoming.empty()) {
                cout << "valid commands: list, exit" << endl;
            }

            prompt_displayed_ = false;
        }
    }

    // Poll each port
    if (poll(port_fds_.data(), port_fds_.size(), 0)) {
        for (size_t i = 0; i < port_fds_.size(); i++) {
            if (port_fds_[i].revents & POLLIN) {
                unique_ptr<Packet> packet;

                if (packet = receivePacket((int) i)) {
                    // We are reading a packet from a switch
                    processPacket((int) i, packet);
                }
            }
        }
    }
}

void NetworkNode::exit() {
    list();
    node_ok = false;
}

bool NetworkNode::ok() {
    return node_ok;
}

int NetworkNode::getId() const {
    return id_;
}

void NetworkNode::setPort(int port, shared_ptr<Port> port_ptr) {
    ports_.at((size_t) port) = std::move(port_ptr);
    port_fds_.at((size_t) port) = pollfd { ports_.at((size_t) port)->rfd(), POLLIN, 0 };
}

void NetworkNode::setFifoPort(int port, int dst) {
    ports_.at((size_t) port) = shared_ptr<Port>(new FifoPort(id_, dst));
    port_fds_.at((size_t) port) = pollfd { ports_.at((size_t) port)->rfd(), POLLIN, 0 };
}

void NetworkNode::setSocketPort(int port, int dst, const std::string& dst_ip, int dst_port) {
    ports_.at((size_t) port) = shared_ptr<Port>(new SocketPort(id_, dst, dst_ip, dst_port));
    port_fds_.at((size_t) port) = pollfd { ports_.at((size_t) port)->rfd(), POLLIN, 0 };
}

shared_ptr<Port> NetworkNode::getPort(int port) const {
    return ports_.at((size_t) port);
}

void NetworkNode::closePort(int port) {
    ports_.at((size_t) port) = nullptr;
    port_fds_.at((size_t) port) = NULL_POLL_FD;
}

unique_ptr<Packet> NetworkNode::receivePacket(int port) {
    unique_ptr<Packet> packet = getPort(port)->readPacket();
    printf("\rReceived %s\n", packet->toString(port, getId()).c_str());
    prompt_displayed_ = false;
    return packet;
}

void NetworkNode::transmitPacket(int port, const Packet& packet) {
    getPort(port)->writePacket(packet);
    printf("\rTransmitted %s\n", packet.toString(getId(), port).c_str());
    prompt_displayed_ = false;
}
