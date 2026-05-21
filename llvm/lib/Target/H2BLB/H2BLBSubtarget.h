#pragma once

#include "H2BLBISelLowering.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"

namespace llvm {

class H2BLBSubtarget final : public TargetSubtargetInfo {
  H2BLBTargetLowering TLInfo;

public:
  H2BLBSubtarget(const Triple &TT, StringRef CPU, StringRef TuneCPU,
                 StringRef FS, const TargetMachine &TM);

  const TargetLowering *getTargetLowering() const override {
    return &TLInfo;
  }

  const TargetRegisterInfo *getRegisterInfo() const override;
};

} // namespace llvm
