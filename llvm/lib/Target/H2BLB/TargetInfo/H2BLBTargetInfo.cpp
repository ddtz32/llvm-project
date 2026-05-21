#include "H2BLBTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"

using namespace llvm;

Target &llvm::getTheH2BLBTarget() {
  static Target TheH2BLBTarget;
  return TheH2BLBTarget;
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void
LLVMInitializeH2BLBTargetInfo() {
  RegisterTarget<Triple::h2blb> X(getTheH2BLBTarget(),
                                  /*Name=*/"h2blb",
                                  /*Desc=*/"How to build an LLVM backend",
                                  /*BackendName=*/"H2BLB");
}
