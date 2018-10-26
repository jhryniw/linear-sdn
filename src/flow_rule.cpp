#include <flow_rule.h>

using namespace std;

const char DATA_SEP = ' ';

FlowRule::FlowRule() :
    srcIP_lo(0),
    srcIP_hi(1000),
    dstIP_lo(1001),
    dstIP_hi(1001),
    actionType(DROP),
    actionVal(3),
    pri(MIN_PRI),
    pktCount(0)
{

}

FlowRule::FlowRule(int src_lo, int src_hi, int dst_lo, int dst_hi,
                   Action action, int actionVal, int priority, int pktCount) :
   srcIP_lo(src_lo),
   srcIP_hi(src_hi),
   dstIP_lo(dst_lo),
   dstIP_hi(dst_hi),
   actionType(action),
   actionVal(actionVal),
   pri(priority),
   pktCount(pktCount)
{

}

ostream& operator<<(ostream& os, const Action& action)
{
    os << static_cast<int>(action);
    return os;
}

istream& operator>>(istream& is, Action& action)
{
    int int_type;
    is >> int_type;
    action = static_cast<Action>(int_type);
    return is;
}

ostream& operator<<(ostream& os, const FlowRule& fr)
{
    os << fr.srcIP_lo << DATA_SEP << fr.srcIP_hi << DATA_SEP << fr.dstIP_lo << DATA_SEP << fr.dstIP_hi
       << DATA_SEP << fr.actionType << DATA_SEP << fr.actionVal << DATA_SEP << fr.pri << DATA_SEP << fr.pktCount;
    return os;
}

istream& operator>>(istream& is, FlowRule& fr)
{
    is >> fr.srcIP_lo >> fr.srcIP_hi >> fr.dstIP_lo >> fr.dstIP_hi
       >> fr.actionType >> fr.actionVal >> fr.pri >> fr.pktCount;
    return is;
}
