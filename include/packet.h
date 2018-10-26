#ifndef PACKET_H
#define PACKET_H

#include <iostream>
#include <sstream>
#include <memory>

#include <flow_rule.h>

enum PacketType { UNKNOWN, ADMIT, OPEN, ACK, QUERY, ADD, RELAY };

struct Packet {
    PacketType type;
    int srcIP;
    int dstIP;

    Packet();
    explicit Packet(std::istream& is);
    Packet(PacketType type, int src, int dst);

    virtual ~Packet();

    std::string encode() const;
    static std::unique_ptr<Packet> decode(char* msg);

protected:
    virtual void encodePayload(std::ostream& os) const;
};

struct OpenPacket : public Packet {
    int sw, left, right, ipLow, ipHigh;
    explicit OpenPacket(std::istream& is);
    OpenPacket(int sw, int left, int right, int ipLow, int ipHigh);

protected:
    void encodePayload(std::ostream& os) const override;
};

struct AddPacket : public Packet {
    FlowRule flowRule;
    explicit AddPacket(std::istream& is);
    AddPacket(int sw, FlowRule flow_rule);

protected:
    void encodePayload(std::ostream& os) const override;
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
