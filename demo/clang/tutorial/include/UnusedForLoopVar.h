#pragma once
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendAction.h"

class UnusedForLoopVarMatcher final
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  UnusedForLoopVarMatcher() = default;

  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
};

class UnusedForLoopVarVisitor final
    : public clang::RecursiveASTVisitor<UnusedForLoopVarVisitor> {
  clang::ASTContext &Ctx;
  std::set<clang::VarDecl *> LoopVars, UsedLoopVars;

public:
  explicit UnusedForLoopVarVisitor(clang::ASTContext &Ctx) : Ctx(Ctx) {}

  bool TraverseForStmt(clang::ForStmt *ForLoop);
  bool VisitDeclRefExpr(clang::DeclRefExpr *DeclRef);
};

class UnusedForLoopVarConsumer final : public clang::ASTConsumer {
public:
  UnusedForLoopVarConsumer() = default;

  void HandleTranslationUnit(clang::ASTContext &Ctx) override;
};

class UnusedForLoopVarAction final : public clang::PluginASTAction {
public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI,
                    llvm::StringRef InFile) override {
    return std::make_unique<UnusedForLoopVarConsumer>();
  }

  bool ParseArgs(const clang::CompilerInstance &CI,
                 const std::vector<std::string> &Args) override {
    return true;
  }
};
