#include "ToyTargetMachine.h"
#include "TargetInfo/ToyTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
  return RM.value_or(Reloc::Static);
}

ToyTargetMachine::ToyTargetMachine(const Target &T, const Triple &TT,
                                   StringRef CPU, StringRef FS,
                                   const TargetOptions &Options,
                                   std::optional<Reloc::Model> RM,
                                   std::optional<CodeModel::Model> CM,
                                   CodeGenOptLevel OL, bool JIT)
    : CodeGenTargetMachineImpl(
          T, TT.computeDataLayout(Options.MCOptions.getABIName()), TT, CPU, FS,
          Options, getEffectiveRelocModel(RM),
          getEffectiveCodeModel(CM, CodeModel::Small), OL) {
  // TODO
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void LLVMInitializeToyTarget() {
  RegisterTargetMachine<ToyTargetMachine> X(getTheToy32Target());
  RegisterTargetMachine<ToyTargetMachine> Y(getTheToy64Target());
}
