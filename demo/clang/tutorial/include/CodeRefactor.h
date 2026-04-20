#pragma once
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Rewrite/Core/Rewriter.h"

class CodeRefactorMatcher final
    : public clang::ast_matchers::MatchFinder::MatchCallback {
  clang::Rewriter &Rewriter;
  llvm::StringRef NewName;

public:
  CodeRefactorMatcher(clang::Rewriter &Rewriter, llvm::StringRef NewName)
      : Rewriter(Rewriter), NewName(NewName) {}

  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
};

class CodeRefactorConsumer final : public clang::ASTConsumer {
  clang::Rewriter &Rewriter;
  llvm::StringRef ClassName, OldName, NewName;

public:
  CodeRefactorConsumer(clang::Rewriter &Rewriter, llvm::StringRef ClassName,
                       llvm::StringRef OldName, llvm::StringRef NewName)
      : Rewriter(Rewriter), ClassName(ClassName), OldName(OldName),
        NewName(NewName) {}

  void HandleTranslationUnit(clang::ASTContext &Ctx) override;
};
