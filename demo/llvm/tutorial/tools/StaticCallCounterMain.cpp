#include "StaticCallCounter.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/SourceMgr.h"

using namespace llvm;

static cl::OptionCategory
    StaticCallCounterCategory("static call counter options");

static cl::opt<std::string> Input(cl::Positional, cl::desc("<input module"),
                                  cl::Required,
                                  cl::cat(StaticCallCounterCategory));

int main(int argc, char **argv) {
  cl::HideUnrelatedOptions(StaticCallCounterCategory);
  cl::ParseCommandLineOptions(
      argc, argv,
      "Counts the number of static function calls in the input IR file\n");

  llvm_shutdown_obj X;

  SMDiagnostic Err;
  LLVMContext Ctx;
  std::unique_ptr<Module> M = parseIRFile(Input, Err, Ctx);
  if (!M) {
    errs() << "Error reading llvm ir file: " << Input << "\n";
    Err.print(argv[0], errs());
    return -1;
  }

  ModulePassManager MPM;
  MPM.addPass(StaticCallCounterPrinter());

  ModuleAnalysisManager MAM;
  MAM.registerPass([]() { return StaticCallCounter(); });

  PassBuilder PB;
  PB.registerModuleAnalyses(MAM);

  MPM.run(*M, MAM);
}
