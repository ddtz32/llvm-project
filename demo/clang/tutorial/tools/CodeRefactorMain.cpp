#include "CodeRefactor.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

using namespace clang;
using namespace tooling;

static llvm::cl::OptionCategory CodeRefactorCategory("code-refactor options");

static llvm::cl::opt<std::string>
    ClassName("class-name",
              llvm::cl::desc("The name of the class/struct that the "
                             "method to be renamed belongs to"),
              llvm::cl::Required, llvm::cl::cat(CodeRefactorCategory));

static llvm::cl::opt<std::string>
    OldName("old-name",
            llvm::cl::desc("The current name of the method to be renamed"),
            llvm::cl::Required, llvm::cl::cat(CodeRefactorCategory));

static llvm::cl::opt<std::string>
    NewName("new-name",
            llvm::cl::desc("The new name of the method to be renamed"),
            llvm::cl::Required, llvm::cl::cat(CodeRefactorCategory));

namespace {
class CodeRefactorAction final : public clang::PluginASTAction {
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

std::unique_ptr<clang::ASTConsumer>
CodeRefactorAction::CreateASTConsumer(clang::CompilerInstance &CI,
                                      llvm::StringRef InFile) {
  Rewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
  return std::make_unique<CodeRefactorConsumer>(Rewriter, ClassName, OldName,
                                                NewName);
}

void CodeRefactorAction::EndSourceFileAction() {
  PluginASTAction::EndSourceFileAction();

  Rewriter.getEditBuffer(Rewriter.getSourceMgr().getMainFileID())
      .write(llvm::outs());
}
} // namespace

int main(int Argc, const char **Argv) {
  auto OptParser =
      CommonOptionsParser::create(Argc, Argv, CodeRefactorCategory);
  if (!OptParser) {
    llvm::errs() << "Problem constructing CommonOptionsParser "
                 << OptParser.takeError();
    return EXIT_FAILURE;
  }

  ClangTool Tool(OptParser->getCompilations(), OptParser->getSourcePathList());
  return Tool.run(newFrontendActionFactory<CodeRefactorAction>().get());
}
