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

    explicit Controller(int nSwitch);

    /**
     * List switch information and packet stats
     */
    void list() override;

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
};

#endif //CONTROLLER_H
