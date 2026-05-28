#include "H2BLBTargetMachine.h"
#include "H2BLBTargetTransformInfo.h"
#include "TargetInfo/H2BLBTargetInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"

using namespace llvm;

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void LLVMInitializeH2BLBTarget() {
  RegisterTargetMachine<H2BLBTargetMachine> X(getTheH2BLBTarget());
}

H2BLBTargetMachine::H2BLBTargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       std::optional<Reloc::Model> RM,
                                       std::optional<CodeModel::Model> CM,
                                       CodeGenOptLevel OL, bool JIT)
    : CodeGenTargetMachineImpl(T, TT.computeDataLayout(), TT, CPU, FS, Options,
                               RM.value_or(Reloc::Static),
                               CM.value_or(CodeModel::Small), OL) {}

H2BLBTargetMachine::~H2BLBTargetMachine() = default;

const H2BLBSubtarget *
H2BLBTargetMachine::getSubtargetImpl(const Function &F) const {
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  Attribute TuneAttr = F.getFnAttribute("tune-cpu");
  Attribute FSAttr = F.getFnAttribute("target-features");

  std::string CPU =
      CPUAttr.isValid() ? CPUAttr.getValueAsString().str() : TargetCPU;
  std::string TuneCPU =
      TuneAttr.isValid() ? TuneAttr.getValueAsString().str() : CPU;
  std::string FS =
      FSAttr.isValid() ? FSAttr.getValueAsString().str() : TargetFS;

  SmallString<128> Key;
  raw_svector_ostream(Key) << CPU << TuneCPU << FS;
  auto &Subtarget = SubtargetMap[Key];
  if (!Subtarget) {
    Subtarget =
        std::make_unique<H2BLBSubtarget>(TargetTriple, CPU, TuneCPU, FS, *this);
  }
  return Subtarget.get();
}

TargetTransformInfo
H2BLBTargetMachine::getTargetTransformInfo(const Function &F) const {
  return TargetTransformInfo(std::make_unique<H2BLBTTIImpl>(this, F));
}
