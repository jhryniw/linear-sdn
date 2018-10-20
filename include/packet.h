#ifndef PACKET_H
#define PACKET_H

#include <iostream>
#include <sstream>

struct Packet {
    enum Type { OPEN, ACK, QUERY, ADD, RELAY };

    Type type;
    int srcIP;
    int dstIP;

    std::string encode() const;

    static Packet decode(char* msg);
};

std::ostream& operator<<(std::ostream& os, const Packet::Type& type);

std::istream& operator>>(std::istream& is, Packet::Type& type);

#endif // PACKET_H
