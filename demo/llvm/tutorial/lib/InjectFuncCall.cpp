#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Plugins/PassPlugin.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "inject-func-call"

namespace {
bool injectFuncCallImpl(Module &M) {
  bool Changed = false;

  LLVMContext &Ctx = M.getContext();

  // create following declaration:
  // int printf(char *, ...)
  FunctionType *PrintfTy = FunctionType::get(IntegerType::getInt32Ty(Ctx),
                                             PointerType::getUnqual(Ctx), true);
  FunctionCallee Printf = M.getOrInsertFunction("printf", PrintfTy);
  assert(isa<Function>(Printf.getCallee()));
  Function *PrintfF = cast<Function>(Printf.getCallee());

  Constant *FormatStr = ConstantDataArray::getString(
      Ctx, "Hello from %s, number of arguments: %d\n");

  GlobalVariable *FormatStrVar =
      M.getOrInsertGlobal("FormatStr", FormatStr->getType());
  FormatStrVar->setInitializer(FormatStr);

  for (Function &F : M) {
    if (F.isDeclaration())
      continue;

    IRBuilder<> Builder(&*F.getEntryBlock().getFirstInsertionPt());
    LLVM_DEBUG(dbgs() << " Injecting call to printf inside " << F.getName()
                      << "\n");
    Builder.CreateCall(Printf,
                       {FormatStrVar, Builder.CreateGlobalString(F.getName()),
                        Builder.getInt32(F.arg_size())});

    Changed = true;
  }

  return Changed;
}

struct InjectFuncCall final : public PassInfoMixin<InjectFuncCall> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    bool Changed = injectFuncCallImpl(M);
    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};

PassPluginLibraryInfo getInjectFuncCallPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "InjectFuncCall", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "inject-func-call") {
                    MPM.addPass(InjectFuncCall());
                    return true;
                  }
                  return false;
                });
          }};
}

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getInjectFuncCallPluginInfo();
}
