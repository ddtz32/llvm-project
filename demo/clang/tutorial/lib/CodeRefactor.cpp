#include "CodeRefactor.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;
using namespace ast_matchers;

void CodeRefactorMatcher::run(const MatchFinder::MatchResult &Result) {
  const auto *Method = Result.Nodes.getNodeAs<CXXMethodDecl>("Method");
  if (Method) {
    // Method->dump();
    Rewriter.ReplaceText(Method->getLocation(), NewName);
  }

  const auto *MemberAccess = Result.Nodes.getNodeAs<MemberExpr>("MemberAccess");
  if (MemberAccess) {
    // MemberAccess->dump();
    Rewriter.ReplaceText(MemberAccess->getMemberLoc(), NewName);
  }
}

void CodeRefactorConsumer::HandleTranslationUnit(clang::ASTContext &Ctx) {
  MatchFinder Finder;
  CodeRefactorMatcher Matcher(Rewriter, NewName);

  DeclarationMatcher MethodMatcher =
      cxxRecordDecl(isSameOrDerivedFrom(hasName(ClassName)),
                    hasMethod(cxxMethodDecl(hasName(OldName)).bind("Method")));
  Finder.addMatcher(MethodMatcher, &Matcher);

  StatementMatcher CallMatcher = cxxMemberCallExpr(
      callee(memberExpr(member(hasName(OldName))).bind("MemberAccess")),
      thisPointerType(cxxRecordDecl(isSameOrDerivedFrom(hasName(ClassName)))));
  Finder.addMatcher(CallMatcher, &Matcher);

  Finder.matchAST(Ctx);
}


namespace {

class CodeRefactorAction final : public clang::PluginASTAction {
  clang::Rewriter Rewriter;
  std::string ClassName, OldName, NewName;

  void PrintHelp(llvm::raw_ostream &OS) const {
    OS << "Help for CodeRefactor plguin\n";
  }

public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI,
                    llvm::StringRef InFile) override;

  bool ParseArgs(const clang::CompilerInstance &CI,
                 const std::vector<std::string> &Args) override;

  void EndSourceFileAction() override;
};

std::unique_ptr<clang::ASTConsumer>
CodeRefactorAction::CreateASTConsumer(clang::CompilerInstance &CI,
                                      llvm::StringRef InFile) {
  Rewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
  return std::make_unique<CodeRefactorConsumer>(Rewriter, ClassName, OldName,
                                                NewName);
}

bool CodeRefactorAction::ParseArgs(const clang::CompilerInstance &CI,
                                   const std::vector<std::string> &Args) {
  auto &D = CI.getDiagnostics();

  for (int i = 0, e = Args.size(); i < e; i++) {
    llvm::StringRef Arg = Args[i];
    llvm::errs() << "CodeRefactor arg = " << Arg;

    if (Arg == "-class-name") {
      if (i + 1 >= e) {
        unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Error,
                                            "missing -class-name argument");
        D.Report(DiagID);
        return false;
      }
      ClassName = Args[++i];
      llvm::errs() << ", " << ClassName;
    } else if (Arg == "-old-name") {
      if (i + 1 >= e) {
        unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Error,
                                            "missing -old-name argument");
        D.Report(DiagID);
        return false;
      }
      OldName = Args[++i];
      llvm::errs() << ", " << OldName;
    } else if (Arg == "-new-name") {
      if (i + 1 >= e) {
        unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Error,
                                            "missing -new-name argument");
        D.Report(DiagID);
        return false;
      }
      NewName = Args[++i];
      llvm::errs() << ", " << NewName;
    }

    llvm::errs() << "\n";
  }

  if (ClassName.empty()) {
    unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Error,
                                        "missing -class-name argument");
    D.Report(DiagID);
    return false;
  }

  if (OldName.empty()) {
    unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Error,
                                        "missing -old-name argument");
    D.Report(DiagID);
    return false;
  }

  if (NewName.empty()) {
    unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Error,
                                        "missing -new-name argument");
    D.Report(DiagID);
    return false;
  }

  // llvm::errs() << "class-name: " << ClassName << ", old-name: " << OldName
  //              << ", new-name: " << NewName << "\n";

  if (llvm::find_if(Args, [](StringRef Arg) { return Arg == "help"; }) !=
      Args.end())
    PrintHelp(llvm::errs());
  return true;
}

void CodeRefactorAction::EndSourceFileAction() {
  PluginASTAction::EndSourceFileAction();

  Rewriter.getEditBuffer(Rewriter.getSourceMgr().getMainFileID())
      .write(llvm::outs());
}

} // namespace

static FrontendPluginRegistry::Add<CodeRefactorAction>
    X("code-refactor", "change the name of a class method");
