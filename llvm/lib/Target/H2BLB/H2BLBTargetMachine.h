#pragma once

#include "H2BLBSubtarget.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"

namespace llvm {

class H2BLBTargetMachine final : public CodeGenTargetMachineImpl {
  mutable StringMap<std::unique_ptr<H2BLBSubtarget>> SubtargetMap;

public:
  H2BLBTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     std::optional<Reloc::Model> RM,
                     std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                     bool JIT);

  ~H2BLBTargetMachine() override;

  const TargetSubtargetInfo *getSubtargetImpl(const Function &F) const override;
};

} // namespace llvm
