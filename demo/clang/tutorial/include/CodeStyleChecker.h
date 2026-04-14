#pragma once

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/FrontendAction.h"

class CodeStyleCheckerVisitor final
    : public clang::RecursiveASTVisitor<CodeStyleCheckerVisitor> {
  clang::ASTContext &Ctx;

  void checkNoUnderscoreInName(clang::NamedDecl *Decl);
  void checkNameStartsWithLowerCase(clang::NamedDecl *Decl);
  void checkNameStartsWithUpperCase(clang::NamedDecl *Decl);

public:
  explicit CodeStyleCheckerVisitor(clang::ASTContext &Ctx) : Ctx(Ctx) {}
  bool VisitCXXRecordDecl(clang::CXXRecordDecl *Decl);
  bool VisitFunctionDecl(clang::FunctionDecl *Decl);
  bool VisitVarDecl(clang::VarDecl *Decl);
  bool VisitFieldDecl(clang::FieldDecl *Decl);
};

class CodeStyleCheckerComsumer final : public clang::ASTConsumer {
  bool MainFileOnly;

public:
  CodeStyleCheckerComsumer(bool MainFileOnly) : MainFileOnly(MainFileOnly) {}

  void HandleTranslationUnit(clang::ASTContext &Ctx) override;
};

class CodeStyleCheckerAction final : public clang::PluginASTAction {
  bool MainFileOnly = true;

public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI,
                    llvm::StringRef InFile) override {
    return std::make_unique<CodeStyleCheckerComsumer>(MainFileOnly);
  }

  bool ParseArgs(const clang::CompilerInstance &CI,
                 const std::vector<std::string> &Args) override;

  void PrintHelp(llvm::raw_ostream &OS) const {
    OS << "Help for CodeStyleChecker plguin\n";
  }
};
