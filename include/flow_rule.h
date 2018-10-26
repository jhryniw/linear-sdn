#ifndef FLOW_RULE_H
#define FLOW_RULE_H

#include <iostream>

enum Action { DELIVER, FORWARD, DROP };

struct FlowRule {
    static constexpr int MIN_PRI = 4;

    int srcIP_lo = 0, srcIP_hi = 1000, dstIP_lo = -1, dstIP_hi = -1;
    Action actionType = FORWARD;
    int actionVal = 0;
    int pri = MIN_PRI;
    int pktCount = 0;

    FlowRule() = default;

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
