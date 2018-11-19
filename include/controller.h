#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <utils.h>
#include <switch.h>
#include <network_node.h>

/** Minimal structure containing basic switch information */
struct SwitchInfo {
    int id;
    int left;
    int right;
    int lowIP;
    int highIP;

    SwitchInfo();
    SwitchInfo(int id, int left, int right, int low_ip, int high_ip);
};

class Controller : public NetworkNode {
public:

    Controller(int nSwitch, int listen_port);

    ~Controller() override;

    /**
     * List switch information and packet stats
     */
    void list() override;

    /**
     * The main loop, reads and responds to incoming packets
     */
    void loop() override;

    /**
     * Main entry point for incoming packets
     * @param port the port from which the packet was received
     * @param packet the packet
     */
    void processPacket(int port, const std::unique_ptr<Packet>& packet) override;

    /**
     * @return the controller type
     */
    std::string getType() const override;

private:
    pollfd listen_fd_;
    std::vector<pollfd> unknown_conns_;
    std::vector<std::shared_ptr<Port>> unknown_ports_;

    std::vector<SwitchInfo> switches_;
    int open_count_ = 0, ack_count_ = 0, query_count_ = 0, add_count_ = 0;

    /**
     * Register a new switch and send back an ACK message
     * @param port the switch port number (should also its id)
     * @param op the open packet
     */
    void handleOpenPacket(int port, const OpenPacket* op);

    /**
     * Handle a switch query and send back an appropriate ADD message
     * @param port the switch port number (should also its id)
     * @param qp the query packet
     */
    void handleQueryPacket(int port, const Packet* qp);

    /**
     * Binds listening socket
     * @param port a networking port to listen on
     */
    void bind(int listen_port);

    /**
     * Activate listening socket
     */
    void listen();

    /**
     * Accept incoming switch connections
     */
    void accept();

    /**
     * Register an unknown socket connection (no switch info)
     * @param sfd the socket file descriptor
     */
    void addUnknownConnection(int sfd);

    /**
     * Remove an unknown socket connection
     * @param port the unknown port number
     */
    void removeUnknownConnection(int port);
};

#endif //CONTROLLER_H
