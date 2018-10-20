#include <packet.h>

using namespace std;

string Packet::encode() const {
    ostringstream oss;
    oss << type << srcIP << dstIP;
    return oss.str();
}

Packet Packet::decode(char* msg) {
    Packet p;
    istringstream iss(msg);
    iss >> p.type >> p.srcIP >> p.dstIP;
    return p;
}

ostream& operator<<(ostream& os, const Packet::Type& type)
{
    os << static_cast<int>(type);
    return os;
}

istream& operator>>(istream& is, Packet::Type& type)
{
    int int_type;
    is >> int_type;
    type = static_cast<Packet::Type>(int_type);
    return is;
}
