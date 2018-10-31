#include <packet.h>

using namespace std;

const char* DATA_SEP = " ";

inline string NodeString(int node_id) {
    if (node_id == -1) {
        return "null";
    }

    if (node_id == 0) {
        return "cont";
    }

    return "sw" + to_string(node_id);
}

Packet::Packet() :
        type(PacketType::UNKNOWN),
        srcIP(-1),
        dstIP(-1)
{

}

Packet::Packet(istream& is) :
    type(PacketType::UNKNOWN)
{
    is >> srcIP >> dstIP;
}

Packet::Packet(PacketType type, int src, int dst) :
    type(type),
    srcIP(src),
    dstIP(dst)
{

}

string Packet::encode() const {
    ostringstream oss;

    // Encode message header
    oss << type << DATA_SEP << srcIP << DATA_SEP << dstIP;

    // Encode message payload
    encodePayload(oss);

    return oss.str();
}

string Packet::toString(int src_id, int dst_id) const {
    ostringstream oss;
    oss << "(src= " << NodeString(src_id) << ", dest= " << NodeString(dst_id)
        << ") [" << ToString(type) << "]";

    if (type == QUERY || type == RELAY) {
        oss << " header= (srcIP= " << srcIP << ", destIP= " << dstIP << ")";
    }

    return oss.str();
}

unique_ptr<Packet> Packet::decode(char* msg) {
    unique_ptr<Packet> p;
    PacketType type;
    istringstream iss(msg);

    iss >> type;

    switch (type) {
        case OPEN:
            p = unique_ptr<Packet>(new OpenPacket(iss));
            break;
        case ADD:
            p = unique_ptr<Packet>(new AddPacket(iss));
            break;
        case ACK:
        case QUERY:
        case RELAY:
        case ADMIT:
        case UNKNOWN:
            p = unique_ptr<Packet>(new Packet(iss));
            p->type = type;
            break;
    }

    return p;
}

void Packet::encodePayload(std::ostream& os) const {}

OpenPacket::OpenPacket(istream& is) :
    Packet(is)
{
    type = PacketType::OPEN;
    is >> sw >> left >> right >> ipLow >> ipHigh;
}

OpenPacket::OpenPacket(int sw, int left, int right, int ipLow, int ipHigh) :
    Packet(PacketType::OPEN, sw, 0),
    sw(sw),
    left(left),
    right(right),
    ipLow(ipLow),
    ipHigh(ipHigh)
{

}

string OpenPacket::toString(int src_id, int dst_id) const {
    char s[256];
    sprintf(s, "%s\n\t(port0= %s, port1= %s, port2= %s, port3= %d-%d)", Packet::toString(src_id, dst_id).c_str(),
            NodeString(0).c_str(), NodeString(left).c_str(), NodeString(right).c_str(), ipLow, ipHigh);

    return string(s);
}

void OpenPacket::encodePayload(std::ostream& os) const
{
    os << DATA_SEP << sw << DATA_SEP << left << DATA_SEP << right
       << DATA_SEP << ipLow << DATA_SEP << ipHigh;
}

AddPacket::AddPacket(std::istream &is) :
    Packet(is)
{
    type = PacketType::ADD;
    is >> flowRule;
}

AddPacket::AddPacket(int sw, FlowRule flow_rule) :
    Packet(PacketType::ADD, 0, sw),
    flowRule(flow_rule)
{

}

string AddPacket::toString(int src_id, int dst_id) const {
    char s[256];
    sprintf(s, "%s\n\t(srcIP= %d-%d, destIP= %d-%d, action= %s:%d, pri= %d, pktCount= %d)",
            Packet::toString(src_id, dst_id).c_str(), flowRule.srcIP_lo, flowRule.srcIP_hi,
            flowRule.dstIP_lo, flowRule.dstIP_hi, ToString(flowRule.actionType), flowRule.actionVal,
            flowRule.pri, flowRule.pktCount);

    return string(s);
}

void AddPacket::encodePayload(std::ostream& os) const
{
    os << DATA_SEP << flowRule;
}

ostream& operator<<(ostream& os, const PacketType& type)
{
    os << static_cast<int>(type);
    return os;
}

istream& operator>>(istream& is, PacketType& type)
{
    int int_type;
    is >> int_type;
    type = static_cast<PacketType>(int_type);
    return is;
}
