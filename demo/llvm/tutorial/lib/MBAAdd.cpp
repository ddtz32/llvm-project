#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Plugins/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

#define DEBUG_TYPE "mba-add"

STATISTIC(SubstCount, "The # of substituted instructions");

namespace {

bool runOnBasicBlock(BasicBlock &BB) {
  bool Changed = false;

  for (auto Inst = BB.begin(), E = BB.end(); Inst != E; Inst++) {
    BinaryOperator *BinOp = dyn_cast<BinaryOperator>(Inst);
    if (!BinOp)
      continue;

    if (BinOp->getOpcode() != Instruction::Add ||
        !BinOp->getType()->isIntegerTy() ||
        BinOp->getType()->getIntegerBitWidth() != 8)
      continue;

    IRBuilder<> Builder(BinOp);
    Constant *Val2 = ConstantInt::get(BinOp->getType(), 2),
             *Val23 = ConstantInt::get(BinOp->getType(), 23),
             *Val39 = ConstantInt::get(BinOp->getType(), 39),
             *Val111 = ConstantInt::get(BinOp->getType(), 111),
             *Val151 = ConstantInt::get(BinOp->getType(), 151);

    auto *ValXor =
             Builder.CreateXor(BinOp->getOperand(0), BinOp->getOperand(1)),
         *ValMul =
             Builder.CreateMul(Val2, Builder.CreateAnd(BinOp->getOperand(0),
                                                       BinOp->getOperand(1)));
    Instruction *NewValue = BinaryOperator::CreateAdd(
        Builder.CreateMul(
            Builder.CreateAdd(
                Builder.CreateMul(Builder.CreateAdd(ValXor, ValMul), Val39),
                Val23),
            Val151),
        Val111);

    LLVM_DEBUG(dbgs() << *BinOp << "  ->");
    ReplaceInstWithInst(&BB, Inst, NewValue);
    LLVM_DEBUG(dbgs() << *NewValue << "\n");
    Changed = true;
    SubstCount++;
  }

  return Changed;
}

struct MBAAdd final : public PassInfoMixin<MBAAdd> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    bool Changed = false;

    for (BasicBlock &BB : F)
      Changed |= runOnBasicBlock(BB);

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::none();
  }

  static bool isRequired() { return true; }
};

PassPluginLibraryInfo getMBAAddPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MBAAdd", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "mba-add") {
                    FPM.addPass(MBAAdd());
                    return true;
                  }
                  return false;
                });
          }};
}

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getMBAAddPluginInfo();
}
