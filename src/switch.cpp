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

    // Send OPEN packet to the controller
    OpenPacket open_p(getId(), swj, swk);
    getPort(0)->writePacket(open_p);

}

Switch::~Switch()
{
    traffic_file_.close();
}

void Switch::list()
{
    cout << "Switch Information" << endl;
}

void Switch::loop()
{
    if (!ack_received_) {
        // Pass
    } else if (!traffic_file_.eof()) {
        Packet traffic;

        if (nextPacket(traffic)) {
            processPacket(3, traffic);
        }
    }

    NetworkNode::loop();
}

void Switch::processPacket(int port, const Packet& packet)
{
    switch (packet.type) {
        case OPEN:
            break;
        case ACK:
            ack_received_ = true;
            break;
        case QUERY:
        case ADD:
        case RELAY:
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
        if (traffic.size() == 0 || traffic.find("#") == 0) {
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
