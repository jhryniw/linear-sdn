#ifndef PACKET_H
#define PACKET_H

#include <iostream>
#include <sstream>
#include <memory>

#include <flow_rule.h>

enum PacketType { UNKNOWN, ADMIT, OPEN, ACK, QUERY, ADD, RELAY, CLOSE };

struct Packet {

    // Type and header information
    PacketType type;
    int srcIP;
    int dstIP;

    // Constructors
    Packet();
    explicit Packet(std::istream& is);
    Packet(PacketType type, int src, int dst);

    virtual ~Packet() = default;

    std::string encode() const;
    static std::unique_ptr<Packet> decode(const char* msg);

    virtual std::string toString(int src_id, int dst_id) const;

protected:
    virtual void encodePayload(std::ostream& os) const;
};

struct OpenPacket : public Packet {
    // Switch information payload
    int sw, left, right, ipLow, ipHigh;

    // Constructors
    explicit OpenPacket(std::istream& is);
    OpenPacket(int sw, int left, int right, int ipLow, int ipHigh);

    std::string toString(int src_id, int dst_id) const override;

protected:
    void encodePayload(std::ostream& os) const override;
};

struct AddPacket : public Packet {
    // Flow rule payload
    FlowRule flowRule;

    // Constructors
    explicit AddPacket(std::istream& is);
    AddPacket(int sw, FlowRule flow_rule);

    std::string toString(int src_id, int dst_id) const override;
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
        case CLOSE: return "CLOSE";
        default:    return "UNKNOWN";
    }
}

#endif // PACKET_H
