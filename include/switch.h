#ifndef SWITCH_H
#define SWITCH_H

#include <list>
#include <fstream>
#include <iostream>
#include <cstring>

#include <packet.h>
#include <network_node.h>

class Switch : public NetworkNode {
public:
    static constexpr int CONT_PORT = 0;
    static constexpr int NULL_SW = -1;

    Switch(int id, int swj, int swk, std::string tf_path, int ip_low, int ip_high);

    ~Switch();

    virtual void list();
    virtual void loop();
    virtual void processPacket(int port, const Packet& packet);

private:
    std::list<FlowRule> flow_table_;
    std::ifstream traffic_file_;
    int ip_low_, ip_high_;
    bool ack_received_;

    bool nextPacket(Packet& packet);
};

#endif //SWITCH_H
