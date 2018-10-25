#ifndef FLOW_RULE_H
#define FLOW_RULE_H

enum Action { FORWARD, DROP };

struct FlowRule {
    static constexpr int MIN_PRI = 4;

    int srcIP_lo, srcIP_hi, dstIP_lo, dstIP_hi;
    Action actionType;
    int actionVal;
    int pri = MIN_PRI;
    int pktCount = 0;

    FlowRule(int src_lo, int src_hi, int dst_lo, int dst_hi,
             Action action, int actionVal, int priority = MIN_PRI, int pktCount = 0);
};

inline const char* ToString(Action a) {
    switch (a)
    {
        case FORWARD:  return "FORWARD";
        case DROP:   return "DROP";
        default:    return "UNKNOWN";
    }
}

#endif //FLOW_RULE_H
