#pragma once
#include "llvm/ADT/MapVector.h"
#include "llvm/IR/PassManager.h"

using StaticCallCounterResult = llvm::MapVector<llvm::Function *, unsigned>;

struct StaticCallCounter final
    : public llvm::AnalysisInfoMixin<StaticCallCounter> {
  using Result = StaticCallCounterResult;

  Result run(llvm::Module &M, llvm::ModuleAnalysisManager &);

  static bool isRequired() { return true; }

private:
  static llvm::AnalysisKey Key;
  friend AnalysisInfoMixin<StaticCallCounter>;
};

struct StaticCallCounterPrinter final
    : public llvm::PassInfoMixin<StaticCallCounterPrinter> {
  llvm::PreservedAnalyses run(llvm::Module &M,
                              llvm::ModuleAnalysisManager &MAM);

  static bool isRequired() { return true; }
};
