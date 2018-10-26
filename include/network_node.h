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
#include <port.h>

#define MAX_BUF 128

static constexpr int MAX_IP = 1000;

class NetworkNode {
public:

    NetworkNode(int id, int max_ports);

    virtual void loop();
    virtual void list() = 0;
    virtual void exit();
    virtual bool ok();

    virtual void processPacket(int port, const std::unique_ptr<Packet>& packet) = 0;
    virtual std::string getType() const = 0;

    int getId() const;

protected:
    bool node_ok;

    void setPort(int port, int dst);
    std::shared_ptr<Port> getPort(int port) const;

private:
    int id_;
    std::vector<pollfd> port_fds_;
    std::vector<std::shared_ptr<Port>> ports_;
    pollfd stdin_fd_;
    bool prompt_displayed_;
};

#endif //NETWORK_NODE_H
