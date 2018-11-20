#ifndef SWITCH_H
#define SWITCH_H

#include <list>
#include <deque>
#include <unordered_set>
#include <fstream>
#include <iostream>
#include <cstring>
#include <memory>
#include <functional>

#include <utils.h>
#include <packet.h>
#include <flow_rule.h>
#include <network_node.h>
#include <chrono>

using sys_time = std::chrono::time_point<std::chrono::system_clock>;

class Switch : public NetworkNode {
public:
    static constexpr int CONT_PORT = 0;
    static constexpr int LEFT_PORT = 1;
    static constexpr int RIGHT_PORT = 2;
    static constexpr int NULL_SW = -1;

    Switch(int id, int swj, int swk, const std::string& tf_path, int ip_low, int ip_high,
           const std::string& server_ip, int server_port);

    ~Switch() override;

    /**
     * Prints the flow table and packet stats
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
     * @return the switch identifier
     */
    std::string getType() const override;

private:
    enum SwitchState { PRE_ACK, DELAY, NORMAL };

    struct Traffic {
        bool isValid;
        bool isDelay;
        int delayMs;
        Packet packet;
    };

    /** The switch state */
    SwitchState state_;

    /** The flow table */
    std::list<FlowRule> flow_table_;

    /** A queue of unhandled packets (with outstanding queries) */
    std::deque<Packet> packet_queue_;

    /** A set of serialized queries to prevent requerying */
    std::unordered_set<std::string> query_set_;

    std::ifstream traffic_file_;
    int ip_low_, ip_high_;

    // Packet counts
    int admit_count_ = 0, ack_count_ = 0, add_rule_count_ = 0, relay_in_count_ = 0;
    int open_count_ = 0, query_count_ = 0, relay_out_count_ = 0;

    // Delay info
    sys_time delay_end_;

    /**
     * Handles the RELAY and ADMIT packets based on the flow table
     * @param p the RELAY or ADMIT packet
     */
    void handleNormalPacket(Packet* p);

    /**
     * Registers new flow rules from incoming ADD packets
     * @param ap the add packet
     */
    void handleAddPacket(AddPacket* ap);

    /**
     * Adds a new rule to the flow table
     * @param flow_rule the rule
     */
    void addRule(FlowRule flow_rule);

    /**
     * Return a rule associated with the packet
     * @param packet the packet
     * @return the rule, otherwise nullptr
     */
    FlowRule* matchRule(const Packet& packet);

    /**
     * Process a packet from the packet queue and/or the traffic file
     */
    void processTraffic();

    /**
     * Fetch the next traffic instruction from the traffic file
     * May return either EOF (traffic is not valid), the next packet or a delay
     * @return Traffic struct with next packet or delay or EOF
     */
    Traffic nextTraffic();

    /**
     * Sets the switch into DELAY mode for the next delay_ms milliseconds
     * @param delay_ms The delay time in milliseconds
     */
    void startDelay(long delay_ms);

    /**
     * Polls if the delay has expired
     * @return true if delay has expired
     */
    bool isDelayExpired();
};

#endif //SWITCH_H
