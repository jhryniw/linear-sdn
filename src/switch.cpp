#include <switch.h>

using namespace std;

Switch::Switch(int id, int swj, int swk, string tf_path, int ip_low, int ip_high) :
    NetworkNode(id, 4),
    traffic_file_(tf_path),
    ip_low_(ip_low),
    ip_high_(ip_high),
    ack_received_(false)
{
    setPort(0, CONT_PORT);
    setPort(1, swj);
    setPort(2, swk);

    // Install initial flow rule
    flow_table_.emplace_back(0, MAX_IP, ip_low, ip_high, Action::FORWARD, 3);

    // Send OPEN packet to the controller
    OpenPacket open_p(getId(), swj, swk, ip_low, ip_high);
    getPort(0)->writePacket(open_p);
}

Switch::~Switch()
{
    traffic_file_.close();
}

void Switch::list()
{
    int index = 0;

    cout << "Flow table:\n";
    for(const auto& rule : flow_table_) {
        printf("[%d] (srcIP= %d-%d, destIP= %d-%d action= %s:%d, pri= %d, pktCount= %d)\n",
                index, rule.srcIP_lo, rule.srcIP_hi, rule.dstIP_lo, rule.dstIP_hi, ToString(rule.actionType),
                rule.actionVal, rule.pri, rule.pktCount);
        index++;
    }

    cout << "\nPacket Stats:\n";
    printf("Received:    ADMIT:%d, ACK:%d, ADDRULE:%d, RELAYIN:%d\n", admit_count_, ack_count_, add_rule_count_, relay_in_count_);
    printf("Transmitted: OPEN:%d, QUERY%d, RELAYOUT:%d\n", open_count_, query_count_, relay_out_count_);
    cout << endl;
}

void Switch::loop()
{
    if (!ack_received_) {
        // Pass
    } else if (!traffic_file_.eof()) {
        Packet traffic;

        if (nextPacket(traffic)) {
            admit_count_++;
            processPacket(3, unique_ptr<Packet>(new Packet(traffic)));
        }
    }

    NetworkNode::loop();
}

void Switch::processPacket(int port, const std::unique_ptr<Packet>& packet)
{
    switch (packet->type) {
        case OPEN:
            break;
        case ACK:
            ack_count_++;
            ack_received_ = true;
            break;
        case QUERY:
            query_count_++;
            break;
        case ADD:
            add_rule_count_++;
            break;
        case RELAY:
            relay_in_count_++;
            break;
        case UNKNOWN:
            break;
    }
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
