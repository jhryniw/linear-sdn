#ifndef NETWORK_NODE_H
#define NETWORK_NODE_H

#include <vector>
#include <string>
#include <cstring>
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

    /**
     * The main loop. Polls stdin for commands and all open ports for incoming packets
     */
    virtual void loop();

    virtual void list() = 0;

    /**
     * By default, calls list then makes the node not ok
     */
    virtual void exit();

    /**
     * @return If the node is still ok (has not shutdown)
     */
    virtual bool ok();

    /**
     * Main entry point for incoming packets
     * @param port the port from which the packet was received
     * @param packet the packet
     */
    virtual void processPacket(int port, const std::unique_ptr<Packet>& packet) = 0;

    /**
     * @return the node type
     */
    virtual std::string getType() const = 0;

    /**
     * @return the node id
     */
    int getId() const;

protected:
    bool node_ok;

    /**
     * Opens a new port
     * @param port the port number (0-indexed)
     * @param dst the destination node id
     */
    void setPort(int port, int dst);

    /**
     * Get an open port
     * @param port the port number (0-indexed)
     * @return a pointer to the port
     */
    std::shared_ptr<Port> getPort(int port) const;

    /**
     * Sends a packet on a port.
     * @param port the port number
     * @param packet the packet to send
     */
    void transmitPacket(int port, const Packet& packet);

private:
    int id_;
    std::vector<pollfd> port_fds_;
    std::vector<std::shared_ptr<Port>> ports_;
    pollfd stdin_fd_;
    bool prompt_displayed_;
    char buf_[MAX_BUF];
};

#endif //NETWORK_NODE_H
