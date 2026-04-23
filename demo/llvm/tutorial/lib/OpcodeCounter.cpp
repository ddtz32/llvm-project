#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Plugins/PassPlugin.h"
#include "llvm/Support/Format.h"

using namespace llvm;

namespace {

using OpcodeCounterResult = StringMap<unsigned>;

OpcodeCounterResult opcodeCounterImpl(Function &F) {
  OpcodeCounterResult OpcodeMap;

  for (auto I = inst_begin(F), E = inst_end(F); I != E; I++)
    OpcodeMap[I->getOpcodeName()]++;

  return OpcodeMap;
}

struct OpcodeCounter final : public AnalysisInfoMixin<OpcodeCounter> {
  using Result = OpcodeCounterResult;

  Result run(Function &F, FunctionAnalysisManager &) {
    return opcodeCounterImpl(F);
  }

  static bool isRequired() { return true; }

private:
  static AnalysisKey Key;
  friend struct AnalysisInfoMixin<OpcodeCounter>;
};

struct OpcodeCounterPrinter final : public PassInfoMixin<OpcodeCounterPrinter> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);

  static bool isRequired() { return true; }
};

} // namespace

AnalysisKey OpcodeCounter::Key;

PreservedAnalyses OpcodeCounterPrinter::run(Function &F,
                                            FunctionAnalysisManager &FAM) {
  const auto &OpcodeMap = FAM.getResult<OpcodeCounter>(F);

  errs() << "Printing analysis 'OpcodeCounter Pass' for function '"
         << F.getName() << "':\n"
         << "========================================\n"
         << "OpcodeCounter results\n"
            "========================================\n";
  const char *Str1 = "OPCODE", *Str2 = "TIMES USED";
  errs() << format("%-20s %-10s\n", Str1, Str2);

  errs() << "----------------------------------------\n";
  for (const auto &Opcode : OpcodeMap)
    errs() << format("%-20s %-10lu\n", Opcode.first().str().c_str(),
                     Opcode.second);
  errs() << "----------------------------------------\n";

  return PreservedAnalyses::all();
}

static PassPluginLibraryInfo getOpcodeCounterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "OpcodeCounter", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            // register OpcodeCounter analysis, so that we can find it by
            // FAM.getResult<OpcodeCounter>(F);
            PB.registerAnalysisRegistrationCallback(
                [](FunctionAnalysisManager &FAM) {
                  FAM.registerPass([]() { return OpcodeCounter(); });
                });

            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "print<opcode-counter>") {
                    FPM.addPass(OpcodeCounterPrinter());
                    return true;
                  }
                  return false;
                });

            PB.registerVectorizerStartEPCallback(
                [](FunctionPassManager &FPM, OptimizationLevel) {
                  FPM.addPass(OpcodeCounterPrinter());
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getOpcodeCounterPluginInfo();
}
