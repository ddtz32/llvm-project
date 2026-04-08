#pragma once
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/FrontendAction.h"

class PrintSomething final : public clang::RecursiveASTVisitor<PrintSomething> {
public:
  PrintSomething(clang::ASTContext &Ctx) : Ctx(Ctx) {}

  bool VisitCXXRecordDecl(clang::CXXRecordDecl *Decl);

  llvm::StringMap<unsigned> getDeclMap() const { return DeclMap; }

private:
  clang::ASTContext &Ctx;
  llvm::StringMap<unsigned> DeclMap;
};

class PrintSomethingConsumer final : public clang::ASTConsumer {
public:
  PrintSomethingConsumer() = default;

  void HandleTranslationUnit(clang::ASTContext &Ctx) override;
};

class PrintSomethingAction final : public clang::PluginASTAction {
  std::set<std::string> ParsedTemplates;

public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI,
                    llvm::StringRef InFile) override {
    return std::make_unique<PrintSomethingConsumer>();
  }

  bool ParseArgs(const clang::CompilerInstance &CI,
                 const std::vector<std::string> &Args) override;

  void PrintHelp(llvm::raw_ostream &OS) const {
    OS << "Help for PrintSomething plguin\n";
  }
};
