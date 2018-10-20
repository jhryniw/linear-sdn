#ifndef NETWORK_NODE_H
#define NETWORK_NODE_H

#include <vector>
#include <string>
#include <fstream>
#include <memory>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>

#include <packet.h>
#include <fifo.h>

#define MAX_BUF 128

struct FlowRule {
    static constexpr int MIN_PRI = 4;

    enum Action { FORWARD, DROP };

    int srcIP_lo, srcIP_hi, dstIP_lo, dstIP_hi;
    Action actionType;
    int actionVal;
    int pri = MIN_PRI;
    int pktCount = 0;
};

class NetworkNode {
public:

    NetworkNode(int id, int max_ports);

    virtual void loop();
    virtual void list() = 0;
    virtual void exit() = 0;
    virtual void processPacket(const Packet& packet) = 0;

    int getId();

protected:

    void setPort(int port, int dst);
    Fifo getPort(int port);

private:
    int id_;
    std::vector<pollfd> port_fds_;
    std::vector<std::unique_ptr<Fifo>> ports_;
    pollfd stdin_fd_;
};

#endif //NETWORK_NODE_H
