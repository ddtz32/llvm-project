#include "ToyTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

Target &llvm::getTheToy32Target() {
  static Target Toy32Target;
  return Toy32Target;
}

Target &llvm::getTheToy64Target() {
  static Target Toy64Target;
  return Toy64Target;
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void
LLVMInitializeToyTargetInfo() {
  RegisterTarget<Triple::toy32, /*HasJIT=*/false> X(
      getTheToy32Target(), "toy32", "32-bit RISC-V Toy", "Toy");
  RegisterTarget<Triple::toy64, /*HasJIT=*/false> Y(
      getTheToy64Target(), "toy64", "64-bit RISC-V Toy", "Toy");
}
