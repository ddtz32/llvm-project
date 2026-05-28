#include "H2BLBSubtarget.h"

using namespace llvm;

H2BLBSubtarget::H2BLBSubtarget(const Triple &TT, StringRef CPU,
                               StringRef TuneCPU, StringRef FS,
                               const TargetMachine &TM)
    : TargetSubtargetInfo(TT, CPU, TuneCPU, FS, /*PN=*/{}, /*PF=*/{}, /*PD=*/{},
                          /*WPR=*/nullptr, /*WL=*/nullptr, /*RA=*/nullptr,
                          /*IS=*/nullptr, /*OC=*/nullptr, /*FP=*/nullptr),
      TLI(TM, *this) {}

const TargetRegisterInfo *H2BLBSubtarget::getRegisterInfo() const {
  return nullptr;
}
