#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Plugins/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

#define DEBUG_TYPE "mba-sub"

STATISTIC(SubstCount, "The # of substituted instructions");

namespace {

bool runOnBasicBlock(BasicBlock &BB) {
  bool Changed = false;

  for (auto Inst = BB.begin(), E = BB.end(); Inst != E; Inst++) {
    BinaryOperator *BinOp = dyn_cast<BinaryOperator>(Inst);
    if (!BinOp)
      continue;

    if (BinOp->getOpcode() != Instruction::Sub ||
        !BinOp->getType()->isIntegerTy())
      continue;

    IRBuilder<> Builder(BinOp);

    Instruction *NewValue = BinaryOperator::CreateAdd(
        Builder.CreateAdd(BinOp->getOperand(0),
                          Builder.CreateNot(BinOp->getOperand(1))),
        ConstantInt::get(BinOp->getType(), 1));

    // -debug option will dump this print
    LLVM_DEBUG(dbgs() << *BinOp << "  ->");
    ReplaceInstWithInst(&BB, Inst, NewValue);
    LLVM_DEBUG(dbgs() << *NewValue << "\n");
    Changed = true;
    SubstCount++;
  }

  return Changed;
}

struct MBASub final : public PassInfoMixin<MBASub> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    bool Changed = false;

    for (BasicBlock &BB : F) {
      Changed |= runOnBasicBlock(BB);
    }

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};

PassPluginLibraryInfo getMBASubPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MBASub", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "mba-sub") {
                    FPM.addPass(MBASub());
                    return true;
                  }
                  return false;
                });
          }};
}

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getMBASubPluginInfo();
}
