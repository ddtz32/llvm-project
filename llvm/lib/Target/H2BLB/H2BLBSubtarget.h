#pragma once

#include "H2BLBISelLowering.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"

namespace llvm {

class H2BLBSubtarget final : public TargetSubtargetInfo {
  H2BLBTargetLowering TLI;

public:
  H2BLBSubtarget(const Triple &TT, StringRef CPU, StringRef TuneCPU,
                 StringRef FS, const TargetMachine &TM);

  const H2BLBTargetLowering *getTargetLowering() const override { return &TLI; }

  const TargetRegisterInfo *getRegisterInfo() const override;
};

} // namespace llvm
