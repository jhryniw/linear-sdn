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

    void list() override;
    void processPacket(int port, const std::unique_ptr<Packet>& packet) override;

private:
    std::vector<SwitchInfo> switches_;
    int open_count_ = 0, ack_count_ = 0, query_count_ = 0, add_count_ = 0;

    void handleOpenPacket(int port, const OpenPacket* op);
    void handleQueryPacket(int port, const Packet* qp);
};

#endif //CONTROLLER_H
