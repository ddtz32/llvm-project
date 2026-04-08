#pragma once
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Rewrite/Core/Rewriter.h"

class LACommenterMatcher final
    : public clang::ast_matchers::MatchFinder::MatchCallback {
  clang::Rewriter &Rewriter;
  llvm::SmallSet<clang::FullSourceLoc, 8> EditedLocations;

public:
  LACommenterMatcher(clang::Rewriter &Rewriter) : Rewriter(Rewriter) {}

  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
};

class LACommenterConsumer final : public clang::ASTConsumer {
  clang::Rewriter &Rewriter;

public:
  LACommenterConsumer(clang::Rewriter &Rewriter) : Rewriter(Rewriter) {}

  void HandleTranslationUnit(clang::ASTContext &Ctx) override;
};

class LACommenterAction final : public clang::PluginASTAction {
  clang::Rewriter Rewriter;

public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI,
                    llvm::StringRef InFile) override;

  bool ParseArgs(const clang::CompilerInstance &CI,
                 const std::vector<std::string> &Args) override {
    return true;
  }
};
