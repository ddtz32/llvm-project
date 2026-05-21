#pragma once

#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

class H2BLBTargetLowering final : public TargetLowering {
public:
  H2BLBTargetLowering(const TargetMachine &TM, const TargetSubtargetInfo &STI);
};

} // namespace llvm
