#include <flow_rule.h>

FlowRule::FlowRule(int src_lo, int src_hi, int dst_lo, int dst_hi,
                   Action action, int actionVal, int priority, int pktCount) :
   srcIP_lo(src_lo),
   srcIP_hi(src_hi),
   dstIP_lo(dst_lo),
   dstIP_hi(dst_hi),
   actionType(action),
   pri(priority),
   pktCount(pktCount)
{

}
