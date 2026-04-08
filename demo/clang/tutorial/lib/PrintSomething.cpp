#include "PrintSomething.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

bool PrintSomething::VisitCXXRecordDecl(CXXRecordDecl *Decl) {
  auto FullLoc = Ctx.getFullLoc(Decl->getBeginLoc());
  if (!FullLoc.isValid())
    return true;

  if (FullLoc.isMacroID())
    FullLoc = FullLoc.getExpansionLoc();

  DeclMap[FullLoc.getFileEntryRef()->getName()]++;
  return true;
}

void PrintSomethingConsumer ::HandleTranslationUnit(ASTContext &Ctx) {
  PrintSomething Visitor(Ctx);
  Visitor.TraverseDecl(Ctx.getTranslationUnitDecl());

  if (Visitor.getDeclMap().empty()) {
    llvm::errs() << "no declarations found\n";
    return;
  }

  for (const auto &E : Visitor.getDeclMap())
    llvm::errs() << "count: " << E.second << ", file: " << E.first() << "\n";
}

bool PrintSomethingAction ::ParseArgs(const CompilerInstance &CI,
                                      const std::vector<std::string> &Args) {
  for (int i = 0, e = Args.size(); i < e; ++i) {
    StringRef Arg = Args[i];
    llvm::errs() << "PrintSomething arg = " << Arg << "\n";

    auto &D = CI.getDiagnostics();

    if (Arg == "-an-error") {
      unsigned DiagID =
          D.getCustomDiagID(DiagnosticsEngine::Error, "invalid argument '%0'");
      D.Report(DiagID) << Arg;
      return false;
    }

    if (Arg == "-parse-template") {
      if (i + 1 >= e) {
        unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Error,
                                            "missing -parse-template argument");
        D.Report(DiagID);
        return false;
      }
      ++i;
      ParsedTemplates.insert(Args[i]);
    }
  }

  if (llvm::find_if(Args, [](StringRef Arg) { return Arg == "help"; }) !=
      Args.end())
    PrintHelp(llvm::errs());

  return true;
}

static FrontendPluginRegistry::Add<PrintSomethingAction>
    X("print-something", "print something in current translation unit");
