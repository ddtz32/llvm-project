#include "CodeStyleChecker.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

void CodeStyleCheckerVisitor::checkNoUnderscoreInName(NamedDecl *Decl) {
  std::string Name = Decl->getNameAsString();
  auto UnderScorePos = Name.find('_');
  if (UnderScorePos == std::string::npos)
    return;

  std::string Hint = Name;
  auto EndPos = std::remove(Hint.begin(), Hint.end(), '_');
  Hint.erase(EndPos, Hint.end());
  FixItHint FixItHint =
      FixItHint::CreateReplacement(Decl->getSourceRange(), Hint);

  auto &D = Ctx.getDiagnostics();
  unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Warning,
                                      "'_' in names is not allowed");
  D.Report(Decl->getLocation(), DiagID) << FixItHint;
}

void CodeStyleCheckerVisitor::checkNameStartsWithLowerCase(NamedDecl *Decl) {
  std::string Name = Decl->getNameAsString();
  char FirstChar = Name.front();
  if (isLowercase(FirstChar))
    return;

  std::string Hint = Name;
  Hint.front() = isLowercase(FirstChar);
  FixItHint FixItHint =
      FixItHint::CreateReplacement(Decl->getSourceRange(), Hint);

  auto &D = Ctx.getDiagnostics();
  unsigned DiagID =
      D.getCustomDiagID(DiagnosticsEngine::Warning,
                        "Function names should start with lower-case letter");
  D.Report(Decl->getLocation(), DiagID) << FixItHint;
}

void CodeStyleCheckerVisitor::checkNameStartsWithUpperCase(NamedDecl *Decl) {
  std::string Name = Decl->getNameAsString();
  char FirstChar = Name.front();
  if (isUppercase(FirstChar))
    return;

  std::string Hint = Name;
  Hint.front() = toUppercase(FirstChar);
  FixItHint FixItHint =
      FixItHint::CreateReplacement(Decl->getSourceRange(), Hint);

  auto &D = Ctx.getDiagnostics();
  unsigned DiagID = D.getCustomDiagID(
      DiagnosticsEngine::Warning,
      "Type and variable names should start with upper-case letter");
  D.Report(Decl->getLocation(), DiagID) << FixItHint;
}

bool CodeStyleCheckerVisitor::VisitCXXRecordDecl(CXXRecordDecl *Decl) {
  if (Decl->getNameAsString().empty())
    return true;

  checkNameStartsWithUpperCase(Decl);
  checkNoUnderscoreInName(Decl);
  return true;
}

bool CodeStyleCheckerVisitor::VisitFunctionDecl(FunctionDecl *Decl) {
  if (llvm::isa<CXXConversionDecl>(Decl))
    return true;

  checkNameStartsWithLowerCase(Decl);
  checkNoUnderscoreInName(Decl);
  return true;
}

bool CodeStyleCheckerVisitor::VisitVarDecl(VarDecl *Decl) {
  if (llvm::isa<ParmVarDecl>(Decl) && Decl->getNameAsString().empty())
    return true;

  checkNameStartsWithUpperCase(Decl);
  checkNoUnderscoreInName(Decl);
  return true;
}

bool CodeStyleCheckerVisitor::VisitFieldDecl(FieldDecl *Decl) {
  if (Decl->getNameAsString().empty())
    return true;

  checkNameStartsWithUpperCase(Decl);
  checkNoUnderscoreInName(Decl);
  return true;
}

void CodeStyleCheckerComsumer::HandleTranslationUnit(clang::ASTContext &Ctx) {
  CodeStyleCheckerVisitor Visitor(Ctx);
  if (!MainFileOnly)
    Visitor.TraverseDecl(Ctx.getTranslationUnitDecl());
  else {
    for (auto &Decl : Ctx.getTranslationUnitDecl()->decls()) {
      if (!Ctx.getSourceManager().isInMainFile(Decl->getLocation()))
        continue;
      Visitor.TraverseDecl(Decl);
    }
  }
}

bool CodeStyleCheckerAction::ParseArgs(const CompilerInstance &CI,
                                       const std::vector<std::string> &Args) {
  for (llvm::StringRef Arg : Args) {
    if (Arg == "-help")
      PrintHelp(llvm::errs());
    else if (Arg.consume_front("-main-file-only="))
      MainFileOnly = Arg == "true";
    else
      return false;
  }
  return true;
}

static FrontendPluginRegistry::Add<CodeStyleCheckerAction>
    X("csc",
      "check whether class, variable and function names adhere to LLVM's "
      "guidelines");
