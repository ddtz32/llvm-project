#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Plugins/PassPlugin.h"

using namespace llvm;

namespace {

void visitor(Function &F) {
  errs() << "PrintSomething from " << F.getName()
         << ", number of arguments: " << F.arg_size() << "\n";
}

struct PrintSomething final : public llvm::PassInfoMixin<PrintSomething> {
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &) {
    visitor(F);
    return PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};

PassPluginLibraryInfo getPrintSomethingPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PrintSomething", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "print-something") {
                    FPM.addPass(PrintSomething());
                    return true;
                  }
                  return false;
                });
          }};
}

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getPrintSomethingPluginInfo();
}
