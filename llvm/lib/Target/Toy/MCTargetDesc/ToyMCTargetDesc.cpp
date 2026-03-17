#include "ToyMCTargetDesc.h"
#include "TargetInfo/ToyTargetInfo.h"
#include "ToyInstPrinter.h"
#include "ToyMCAsmInfo.h"
#include "llvm/MC/MCInstrAnalysis.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include <cstdint>

#define GET_REGINFO_MC_DESC
#include "ToyGenRegisterInfo.inc"

#define GET_INSTRINFO_MC_DESC
#include "ToyGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "ToyGenSubtargetInfo.inc"

using namespace llvm;

static MCRegisterInfo *createToyMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitToyMCRegisterInfo(X, Toy::X1);
  return X;
}

static MCInstrInfo *createToyMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitToyMCInstrInfo(X);
  return X;
}

static MCSubtargetInfo *createToyMCSubtargetInfo(const Triple &TT,
                                                 StringRef CPU, StringRef FS) {
  if (CPU.empty() || CPU == "generic")
    CPU = TT.isArch64Bit() ? "generic-toy64" : "generic-toy32";

  MCSubtargetInfo *X =
      createToyMCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/ CPU, FS);
  return X;
}

static MCAsmInfo *createToyMCAsmInfo(const MCRegisterInfo &MRI,
                                     const Triple &TT,
                                     const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new ToyMCAsmInfo();
  return MAI;
}

static MCInstPrinter *createToyMCInstPrinter(const Triple &T,
                                             unsigned SyntaxVariant,
                                             const MCAsmInfo &MAI,
                                             const MCInstrInfo &MII,
                                             const MCRegisterInfo &MRI) {
  return new ToyInstPrinter(MAI, MII, MRI);
}

namespace {
class ToyInstrAnalysis : public MCInstrAnalysis {
public:
  ToyInstrAnalysis(const MCInstrInfo *Info) : MCInstrAnalysis(Info) {}

  /// Given a branch instruction try to get the address the branch
  /// targets. Return true on success, and the address in Target.
  bool evaluateBranch(const MCInst &Inst, uint64_t Addr, uint64_t Size,
                      uint64_t &Target) const override;
};
} // namespace

bool ToyInstrAnalysis::evaluateBranch(const MCInst &Inst, uint64_t Addr,
                                      uint64_t Size, uint64_t &Target) const {
  if (isConditionalBranch(Inst)) {
    if (Size != 4)
      return false;
    Target = Addr + Inst.getOperand(2).getImm();
    return true;
  }

  switch (Inst.getOpcode()) {
  default:
    return false;
  case Toy::JAL:
    Target = Addr + Inst.getOperand(1).getImm();
    return true;
  case Toy::JALR:
    // TODO: Not understand
    return false;
  }
}

static MCInstrAnalysis *createToyInstrAnalysis(const MCInstrInfo *Info) {
  return new ToyInstrAnalysis(Info);
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void LLVMInitializeToyTargetMC() {
  for (Target *T : {&getTheToy32Target(), &getTheToy64Target()}) {
    TargetRegistry::RegisterMCRegInfo(*T, createToyMCRegisterInfo);
    TargetRegistry::RegisterMCInstrInfo(*T, createToyMCInstrInfo);
    TargetRegistry::RegisterMCSubtargetInfo(*T, createToyMCSubtargetInfo);
    TargetRegistry::RegisterMCAsmInfo(*T, createToyMCAsmInfo);
    TargetRegistry::RegisterMCInstPrinter(*T, createToyMCInstPrinter);
    TargetRegistry::RegisterMCCodeEmitter(*T, createToyMCCodeEmitter);
    TargetRegistry::RegisterMCAsmBackend(*T, createToyAsmBackend);
    TargetRegistry::RegisterMCInstrAnalysis(*T, createToyInstrAnalysis);
  }
}
