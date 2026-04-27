#include "llvm/ADT/MapVector.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Plugins/PassPlugin.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"

using namespace llvm;

#define DEBUG_TYPE "dynamic-cc"

namespace {
GlobalVariable *createGlobalCounter(Module &M, StringRef Name) {
  LLVMContext &Ctx = M.getContext();
  GlobalVariable *Counter =
      M.getOrInsertGlobal(Name, IntegerType::getInt32Ty(Ctx));
  Counter->setLinkage(GlobalValue::CommonLinkage);
  Counter->setAlignment(Align(4));
  Counter->setInitializer(ConstantInt::get(IntegerType::getInt32Ty(Ctx), 0));
  return Counter;
}

bool dynamicCallCounterImpl(Module &M) {
  bool Changed = false;

  LLVMContext &Ctx = M.getContext();

  MapVector<Function *, GlobalVariable *> CounterMap;

  for (Function &F : M) {
    if (F.isDeclaration())
      continue;

    IRBuilder<> Builder(&*F.getEntryBlock().getFirstInsertionPt());
    std::string Name = "CounterFor_" + F.getName().str();
    GlobalVariable *Counter = createGlobalCounter(M, Name);
    CounterMap.insert({&F, Counter});
    Builder.CreateStore(
        Builder.CreateAdd(Builder.CreateLoad(Counter->getValueType(), Counter),
                          ConstantInt::get(IntegerType::getInt32Ty(Ctx), 1)),
        Counter);

    LLVM_DEBUG(dbgs() << "Instrumented: " << F.getName() << "\n");

    Changed = true;
  }

  if (Changed) {
    FunctionType *PrintfTy = FunctionType::get(
        IntegerType::getInt32Ty(Ctx), PointerType::getUnqual(Ctx), true);
    FunctionCallee Printf = M.getOrInsertFunction("printf", PrintfTy);

    std::string HeaderStr = "========================================\n"
                            "dynamic call counter\n"
                            "========================================\n"
                            "NAME                 #N DIRECT CALLS\n"
                            "----------------------------------------\n";
    Constant *HeaderArr = ConstantDataArray::getString(Ctx, HeaderStr);
    GlobalVariable *Header =
        M.getOrInsertGlobal("CounterResultHeader", HeaderArr->getType());
    Header->setLinkage(GlobalValue::PrivateLinkage);
    Header->setInitializer(HeaderArr);

    Constant *FormatArr = ConstantDataArray::getString(Ctx, "%-20s %-10lu\n");
    GlobalVariable *Formater =
        M.getOrInsertGlobal("CounterResultFormater", FormatArr->getType());
    Formater->setLinkage(GlobalValue::PrivateLinkage);
    Formater->setInitializer(FormatArr);

    Function *PrintfWrapper =
        Function::Create(FunctionType::get(Type::getVoidTy(Ctx), false),
                         GlobalValue::PrivateLinkage, "printf_wrapper", M);
    BasicBlock *Entry = BasicBlock::Create(Ctx, "", PrintfWrapper);
    IRBuilder<> Builder(Entry);

    Builder.CreateCall(Printf, Header);

    for (const auto &Counter : CounterMap) {
      GlobalVariable *FName =
          Builder.CreateGlobalString(Counter.first->getName());
      LoadInst *Load =
          Builder.CreateLoad(Counter.second->getValueType(), Counter.second);
      Builder.CreateCall(Printf, {Formater, FName, Load});
    }

    Builder.CreateRetVoid();
    appendToGlobalDtors(M, PrintfWrapper, 0);
  }

  return Changed;
}

struct DynamicCallCounter final : public PassInfoMixin<DynamicCallCounter> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    bool Changed = dynamicCallCounterImpl(M);
    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};

PassPluginLibraryInfo getDynamicCallCounterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "DynamicCallCounter", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "dynamic-cc") {
                    MPM.addPass(DynamicCallCounter());
                    return true;
                  }
                  return false;
                });
          }};
}

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getDynamicCallCounterPluginInfo();
}
