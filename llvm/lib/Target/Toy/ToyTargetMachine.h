#pragma once

#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"

namespace llvm {

class ToyTargetMachine : public CodeGenTargetMachineImpl {
public:
  ToyTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                   StringRef FS, const TargetOptions &Options,
                   std::optional<Reloc::Model> RM,
                   std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                   bool JIT);
};

} // namespace llvm
