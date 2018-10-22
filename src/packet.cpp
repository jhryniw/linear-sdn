#include <packet.h>

using namespace std;

const char* DATA_SEP = " ";

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

Packet::~Packet() {}

string Packet::encode() const {
    ostringstream oss;

    // Encode message header
    oss << type << DATA_SEP << srcIP << DATA_SEP << dstIP;

    // Encode message payload
    encodePayload(oss);

    return oss.str();
}

Packet Packet::decode(char* msg) {
    Packet* p;
    PacketType type;
    istringstream iss(msg);

    iss >> type;

    switch (type) {
        case OPEN:
            p = new OpenPacket(iss);
            break;
        case ACK:
        case QUERY:
        case ADD:
        case RELAY:
        case UNKNOWN:
            p = new Packet(iss);
            p->type = type;
            break;
    }

    Packet packet = *p;
    delete p;

    return packet;
}

void Packet::encodePayload(std::ostream& os) const {}

OpenPacket::OpenPacket(istream& is) :
    Packet(is)
{
    type = PacketType::OPEN;
    is >> sw >> ipLow >> ipHigh;
}

OpenPacket::OpenPacket(int sw, int ipLow, int ipHigh) :
    Packet(PacketType::OPEN, -1, -1),
    sw(sw),
    ipLow(ipLow),
    ipHigh(ipHigh)
{

}

void OpenPacket::encodePayload(std::ostream& os) const
{
    os << DATA_SEP << sw << DATA_SEP << ipLow << DATA_SEP << ipHigh;
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
