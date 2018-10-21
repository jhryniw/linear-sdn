#include <network_node.h>

using namespace std;

const pollfd NULL_POLL_FD = { -1, 0, 0 };

NetworkNode::NetworkNode(int id, int max_ports) :
    node_ok(true),
    id_(id),
    port_fds_(max_ports, NULL_POLL_FD),
    ports_(max_ports)
{
    stdin_fd_ = pollfd { STDIN_FILENO, POLLIN, 0 };
}

void NetworkNode::loop() {
    // Poll stdin
    if (poll(&stdin_fd_, 1, 0)) {
        if (stdin_fd_.revents & POLLIN) {
            char buf[MAX_BUF];
            read(stdin_fd_.fd, buf, MAX_BUF);
            string incoming(buf);

            // Trim trailing characters
            incoming.erase(incoming.find_last_not_of("\t\f\v\n\r\x7F") + 1);

            if (incoming == "list") {
                list();
            } else if (incoming == "exit") {
                exit();
            }
        }
    }

    // Poll each port
    if (poll(port_fds_.data(), port_fds_.size(), 0)) {
        for (size_t i = 0; i < port_fds_.size(); i++) {
            if (port_fds_[i].revents & POLLIN) {
                // We are reading a packet from a switch
                processPacket(getPort(i)->readPacket());
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

int NetworkNode::getId() {
    return id_;
}

void NetworkNode::setPort(int port, int dst) {
    ports_.at(port) = shared_ptr<Fifo>(new Fifo(id_, dst));
    port_fds_.at(port) = pollfd { ports_.at(port)->rfd(), POLLIN, 0 };
}

shared_ptr<Fifo> NetworkNode::getPort(int port) const {
    return ports_.at(port);
}
