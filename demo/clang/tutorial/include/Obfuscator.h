#pragma once
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Rewrite/Core/Rewriter.h"

class ObfuscatorMatcherForAdd final
    : public clang::ast_matchers::MatchFinder::MatchCallback {
  clang::Rewriter &Rewriter;
  llvm::SmallSet<clang::FullSourceLoc, 8> EditedLocations;

public:
  explicit ObfuscatorMatcherForAdd(clang::Rewriter &Rewriter)
      : Rewriter(Rewriter) {}

  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
};

class ObfuscatorMatcherForSub final
    : public clang::ast_matchers::MatchFinder::MatchCallback {
  clang::Rewriter &Rewriter;
  llvm::SmallSet<clang::FullSourceLoc, 8> EditedLocations;

public:
  explicit ObfuscatorMatcherForSub(clang::Rewriter &Rewriter)
      : Rewriter(Rewriter) {}

  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
};

class ObfuscatorConsumer final : public clang::ASTConsumer {
  clang::Rewriter &Rewriter;

public:
  explicit ObfuscatorConsumer(clang::Rewriter &Rewriter) : Rewriter(Rewriter) {}

  void HandleTranslationUnit(clang::ASTContext &Ctx) override;
};

class ObfuscatorAction final : public clang::PluginASTAction {
  clang::Rewriter Rewriter;

public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI,
                    llvm::StringRef InFile) override;

  bool ParseArgs(const clang::CompilerInstance &CI,
                 const std::vector<std::string> &Args) override {
    return true;
  }

  void EndSourceFileAction() override;
};
