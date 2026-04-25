#include "StaticCallCounter.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Plugins/PassPlugin.h"
#include "llvm/Support/Format.h"

using namespace llvm;

namespace {

StaticCallCounterResult staticCallCounterImpl(Module &M) {
  StaticCallCounterResult Result;

  for (Function &F : M) {
    for (Instruction &I : instructions(F)) {
      if (CallBase *CB = dyn_cast<CallBase>(&I)) {
        Function *Callee = CB->getCalledFunction();
        if (Callee)
          Result[Callee]++;
      }
    }
  }
  return Result;
}

PassPluginLibraryInfo getStaticCallCounterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "StaticCallCounter", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            // registeration for MAM.getResult<StaticCallCounter>(M);
            PB.registerAnalysisRegistrationCallback(
                [](ModuleAnalysisManager &MAM) {
                  MAM.registerPass([]() { return StaticCallCounter(); });
                });

            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "print<static-cc>") {
                    MPM.addPass(StaticCallCounterPrinter());
                    return true;
                  }
                  return false;
                });
          }};
}

} // namespace

StaticCallCounter::Result StaticCallCounter ::run(Module &M,
                                                  ModuleAnalysisManager &) {
  return staticCallCounterImpl(M);
}

AnalysisKey StaticCallCounter::Key;

PreservedAnalyses StaticCallCounterPrinter::run(Module &M,
                                                ModuleAnalysisManager &MAM) {
  StaticCallCounterResult CallCount = MAM.getResult<StaticCallCounter>(M);

  errs() << "========================================\n"
         << "static analysis resutls\n"
         << "========================================\n";
  errs() << format("%-20s %-10s\n", "NAME", "#N DIRECT CALLS")
         << "----------------------------------------\n";
  for (auto &Count : CallCount)
    errs() << format("%-20s %-10lu\n", Count.first->getName().str().c_str(),
                     Count.second);
  errs() << "----------------------------------------\n";

  return PreservedAnalyses::all();
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getStaticCallCounterPluginInfo();
}
