#include <switch.h>

using namespace std;

Switch::Switch(int id, int swj, int swk, const string& tf_path, int ip_low, int ip_high,
               const string& server_ip, int server_port) :
    NetworkNode(id, 4),
    traffic_file_(tf_path),
    ip_low_(ip_low),
    ip_high_(ip_high),
    ack_received_(false)
{
    setSocketPort(CONT_PORT, 0, server_ip, server_port);
    setFifoPort(LEFT_PORT, swj);
    setFifoPort(RIGHT_PORT, swk);

    // Install initial flow rule
    flow_table_.emplace_back(0, MAX_IP, ip_low, ip_high, Action::DELIVER, 3);

    // Send OPEN packet to the controller
    OpenPacket open_p(getId(), swj, swk, ip_low, ip_high);
    transmitPacket(CONT_PORT, open_p);
    open_count_++;
}

Switch::~Switch()
{
    traffic_file_.close();
}

void Switch::list()
{
    int index = 0;

    cout << "\nFlow table:\n";
    for(const auto& rule : flow_table_) {
        printf("[%d] (src= %d-%d, destIP= %d-%d, action= %s:%d, pri= %d, pktCount= %d)\n",
                index, rule.srcIP_lo, rule.srcIP_hi, rule.dstIP_lo, rule.dstIP_hi, ToString(rule.actionType),
                rule.actionVal, rule.pri, rule.pktCount);
        index++;
    }

    cout << "\nPacket Stats:\n";
    printf("Received:    ADMIT:%d, ACK:%d, ADDRULE:%d, RELAYIN:%d\n", admit_count_, ack_count_, add_rule_count_, relay_in_count_);
    printf("Transmitted: OPEN:%d, QUERY:%d, RELAYOUT:%d\n", open_count_, query_count_, relay_out_count_);
    cout << endl;
}

void Switch::loop()
{
    if (!ack_received_) {
        NetworkNode::loop();
        return;
    }

    // Process a packet from the queue. If it still can't be handled it will be added
    // again at the back of the queue
    if (!packet_queue_.empty()) {
        processPacket(3, unique_ptr<Packet>(new Packet(packet_queue_.front())));
        packet_queue_.pop_front();
    }

    // Process another packet from the traffic file
    if (!traffic_file_.eof()) {
        Packet traffic(PacketType::ADMIT, getId(), CONT_PORT);

        if (nextPacket(traffic)) {
            admit_count_++;
            processPacket(3, unique_ptr<Packet>(new Packet(traffic)));
        }
    }

    NetworkNode::loop();
}

std::string Switch::getType() const
{
    stringstream ss;
    ss << "sw" << getId();
    return ss.str();
}

void Switch::processPacket(int port, const std::unique_ptr<Packet>& packet)
{
    switch (packet->type) {
        case RELAY:
            relay_in_count_++;
        case ADMIT:
            handleNormalPacket(packet.get());
            break;
        case ACK:
            ack_count_++;
            ack_received_ = true;
            break;
        case ADD:
            handleAddPacket(dynamic_cast<AddPacket*>(packet.get()));
            break;
        case CLOSE:
            closePort(port);
            break;
        // A switch should not receive the following Packet types. Ignore them
        case OPEN:
        case QUERY:
        case UNKNOWN:
            break;
    }
}

void Switch::handleNormalPacket(Packet* p)
{
    FlowRule* rule;
    rule = matchRule(*p);

    if (rule) {
        if (rule->actionType == Action::FORWARD) {
            // Forward the packet
            Packet relay(*p);
            relay.type = PacketType::RELAY;
            transmitPacket(rule->actionVal, relay);
            relay_out_count_++;
        }

        // We deliver or drop the packet
        rule->pktCount++;
        return;
    }

    // No rule matched, query the controller for a rule
    Packet qp(PacketType::QUERY, p->srcIP, p->dstIP);

    // Do not re-query
    if (query_set_.find(qp.encode()) == query_set_.end()) {
        transmitPacket(CONT_PORT, qp);
        query_set_.emplace(qp.encode());
        query_count_++;
    }

    // Put the packet back into the queue
    packet_queue_.push_back(*p);
}

void Switch::handleAddPacket(AddPacket* ap)
{
    addRule(ap->flowRule);
    add_rule_count_++;
}

void Switch::addRule(FlowRule flow_rule)
{
    // Orders the flow table by priority
    auto it = flow_table_.begin();
    while(it->pri <= flow_rule.pri && it != flow_table_.end()) { it++; }
    flow_table_.insert(it, flow_rule);
}

FlowRule* Switch::matchRule(const Packet& packet)
{
    for(auto& rule : flow_table_) {
        if (between(packet.srcIP, rule.srcIP_lo, rule.srcIP_hi) &&
            between(packet.dstIP, rule.dstIP_lo, rule.dstIP_hi))
            return &rule;
    }

    return nullptr;
}

bool Switch::nextPacket(Packet& packet)
{
    int sw;
    char traffic_buf[256];
    string traffic;

    while (!traffic_file_.eof()) {
        memset(traffic_buf, 0, 256);
        traffic_file_.getline(traffic_buf, 256);

        traffic = string(traffic_buf);

        // Ignore empty or commented lines
        if (traffic.empty() || traffic.find('#') == 0) {
            continue;
        }

        if (sscanf(traffic_buf, "sw%d %d %d", &sw, &packet.srcIP, &packet.dstIP) == 3) {
            if (sw == getId()) {
                return true;
            }
        } else {
            printf("Could not interpret traffic line \"%s\"", traffic_buf);
        }
    }

    return false;
}
