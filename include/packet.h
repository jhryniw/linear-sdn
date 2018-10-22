#ifndef PACKET_H
#define PACKET_H

#include <iostream>
#include <sstream>

enum PacketType { UNKNOWN, OPEN, ACK, QUERY, ADD, RELAY };

struct Packet {
    PacketType type;
    int srcIP;
    int dstIP;

    Packet();
    explicit Packet(std::istream& is);
    Packet(PacketType type, int src, int dst);

    virtual ~Packet();

    std::string encode() const;
    static Packet decode(char* msg);

protected:
    virtual void encodePayload(std::ostream& os) const;
};

struct OpenPacket : public Packet {
    int sw, ipLow, ipHigh;
    explicit OpenPacket(std::istream& is);
    OpenPacket(int sw, int ipLow, int ipHigh);

protected:
    virtual void encodePayload(std::ostream& os) const;
};

std::ostream& operator<<(std::ostream& os, const PacketType& type);

std::istream& operator>>(std::istream& is, PacketType& type);

inline const char* ToString(PacketType v) {
    switch (v)
    {
        case OPEN:  return "OPEN";
        case ACK:   return "ACK";
        case QUERY: return "QUERY";
        case ADD:   return "ADD";
        case RELAY: return "RELAY";
        default:    return "UNKNOWN";
    }
}

#endif // PACKET_H
