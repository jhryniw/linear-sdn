#ifndef SWITCH_H
#define SWITCH_H

#include <list>
#include <fstream>
#include <iostream>
#include <cstring>
#include <memory>

#include <packet.h>
#include <flow_rule.h>
#include <network_node.h>

class Switch : public NetworkNode {
public:
    static constexpr int CONT_PORT = 0;
    static constexpr int NULL_SW = -1;

    Switch(int id, int swj, int swk, std::string tf_path, int ip_low, int ip_high);

    ~Switch();

    void list() override;
    void loop() override;
    void processPacket(int port, const std::unique_ptr<Packet>& packet) override;

private:
    std::list<FlowRule> flow_table_;
    std::ifstream traffic_file_;
    int ip_low_, ip_high_;
    int admit_count_ = 0, ack_count_ = 0, add_rule_count_ = 0, relay_in_count_ = 0;
    int open_count_ = 0, query_count_ = 0, relay_out_count_ = 0;
    bool ack_received_;

    bool nextPacket(Packet& packet);
};

#endif //SWITCH_H
