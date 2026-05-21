#pragma once

#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"

namespace llvm {

class H2BLBTargetMachine final : public CodeGenTargetMachineImpl {
public:
  H2BLBTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     std::optional<Reloc::Model> RM,
                     std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                     bool JIT);

  ~H2BLBTargetMachine() override;
};

} // namespace llvm
