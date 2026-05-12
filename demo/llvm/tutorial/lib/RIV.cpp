#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Plugins/PassPlugin.h"
#include "llvm/Support/Format.h"
#include <deque>
#include <string>

using namespace llvm;

namespace {

using RIVResult = MapVector<const BasicBlock *, SmallPtrSet<Value *, 8>>;

using NodeTy = DomTreeNodeBase<BasicBlock> *;

RIVResult buildRIV(Function &F, NodeTy Root) {
  RIVResult ResultMap;

  std::deque<NodeTy> Worklist;
  Worklist.push_back(Root);

  RIVResult DefinedValuesMap;
  for (BasicBlock &BB : F) {
    for (Instruction &Inst : BB)
      if (Inst.getType()->isIntegerTy())
        DefinedValuesMap[&BB].insert(&Inst);
  }

  for (GlobalVariable &Global : F.getParent()->globals())
    if (Global.getType()->isIntegerTy())
      ResultMap[&F.getEntryBlock()].insert(&Global);

  for (Argument &Arg : F.args())
    if (Arg.getType()->isIntegerTy())
      ResultMap[&F.getEntryBlock()].insert(&Arg);

  while (!Worklist.empty()) {
    NodeTy Parent = Worklist.back();
    Worklist.pop_back();

    auto &ParentDefs = DefinedValuesMap[Parent->getBlock()];
    // 这里直接复制一份, 因为后续对 ResultMap[ChildBB] 导致这里的引用变成悬空指针
    // auto &ParentRIVs = ResultMap[Parent->getBlock()];
    auto ParentRIVs = ResultMap[Parent->getBlock()];

    for (NodeTy Child : *Parent) {
      Worklist.push_back(Child);
      BasicBlock *ChildBB = Child->getBlock();

      ResultMap[ChildBB].insert(ParentDefs.begin(), ParentDefs.end());
      ResultMap[ChildBB].insert(ParentRIVs.begin(), ParentRIVs.end());
    }
  }

  return ResultMap;
}

struct RIV final : public AnalysisInfoMixin<RIV> {
  using Result = RIVResult;
  Result run(Function &F, FunctionAnalysisManager &FAM) {
    DominatorTree *DT = &FAM.getResult<DominatorTreeAnalysis>(F);
    Result Result = buildRIV(F, DT->getRootNode());
    return Result;
  }

private:
  static AnalysisKey Key;
  friend struct AnalysisInfoMixin<RIV>;
};

AnalysisKey RIV::Key;

struct RIVPrinter final : public PassInfoMixin<RIVPrinter> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    RIVResult Result = FAM.getResult<RIV>(F);

    errs() << "==================================================\n"
           << "RIV analysis results\n"
           << "==================================================\n"

           << format("%-10s %-30s", "BB id", "Reachable Integer Values")
           << "\n--------------------------------------------------\n";
    for (const auto &KV : Result) {
      std::string DummyStr;
      raw_string_ostream OS(DummyStr);
      KV.first->printAsOperand(OS, false);
      errs() << format("BB %-12s %-30s\n", DummyStr.c_str(), "");
      for (const Value *Inst : KV.second) {
        DummyStr.clear();
        Inst->print(OS);
        errs() << format("%-12s %-30s\n", "", DummyStr.c_str());
      }
    }

    return PreservedAnalyses::all();
  }
};

PassPluginLibraryInfo getRIVPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "RIV", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "print<riv>") {
                    FPM.addPass(RIVPrinter());
                    return true;
                  }
                  return false;
                });

            PB.registerAnalysisRegistrationCallback(
                [](FunctionAnalysisManager &FAM) {
                  FAM.registerPass([]() { return RIV(); });
                });
          }};
}

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getRIVPluginInfo();
}
