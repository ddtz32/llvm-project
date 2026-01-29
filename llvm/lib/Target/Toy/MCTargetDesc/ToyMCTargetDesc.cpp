#include "ToyMCTargetDesc.h"
#include "TargetInfo/ToyTargetInfo.h"
#include "ToyInstPrinter.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

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
  MCAsmInfo *MAI = new MCAsmInfo();
  return MAI;
}

static MCInstPrinter *createToyMCInstPrinter(const Triple &T,
                                             unsigned SyntaxVariant,
                                             const MCAsmInfo &MAI,
                                             const MCInstrInfo &MII,
                                             const MCRegisterInfo &MRI) {
  return new ToyInstPrinter(MAI, MII, MRI);
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void LLVMInitializeToyTargetMC() {
  for (Target *T : {&getTheToy32Target(), &getTheToy64Target()}) {
    TargetRegistry::RegisterMCRegInfo(*T, createToyMCRegisterInfo);
    TargetRegistry::RegisterMCInstrInfo(*T, createToyMCInstrInfo);
    TargetRegistry::RegisterMCSubtargetInfo(*T, createToyMCSubtargetInfo);
    TargetRegistry::RegisterMCAsmInfo(*T, createToyMCAsmInfo);
    TargetRegistry::RegisterMCInstPrinter(*T, createToyMCInstPrinter);
  }
}
