#ifndef FLOW_RULE_H
#define FLOW_RULE_H

#include <iostream>

enum Action { DELIVER, FORWARD, DROP };

struct FlowRule {
    static constexpr int MIN_PRI = 4;

    int srcIP_lo, srcIP_hi, dstIP_lo, dstIP_hi;
    Action actionType = FORWARD;
    int actionVal;
    int pri = MIN_PRI;
    int pktCount = 0;

    FlowRule();

    FlowRule(int src_lo, int src_hi, int dst_lo, int dst_hi,
             Action action, int actionVal, int priority = MIN_PRI, int pktCount = 0);
};

std::ostream& operator<<(std::ostream& os, const Action& action);
std::istream& operator>>(std::istream& is, Action& action);

std::ostream& operator<<(std::ostream& os, const FlowRule& fr);
std::istream& operator>>(std::istream& is, FlowRule& fr);

inline const char* ToString(Action a) {
    switch (a)
    {
        case DELIVER: return "DELIVER";
        case FORWARD: return "FORWARD";
        case DROP:    return "DROP";
        default:      return "UNKNOWN";
    }
}

#endif //FLOW_RULE_H
