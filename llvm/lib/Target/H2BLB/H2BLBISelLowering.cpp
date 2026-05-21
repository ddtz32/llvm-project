#include "H2BLBISelLowering.h"

using namespace llvm;

#define DEBUG_TYPE "h2blb-lower"

H2BLBTargetLowering::H2BLBTargetLowering(const TargetMachine &TM,
                                         const TargetSubtargetInfo &STI)
    : TargetLowering(TM, STI) {}
