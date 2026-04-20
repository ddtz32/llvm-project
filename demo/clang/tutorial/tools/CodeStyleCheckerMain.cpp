#include "CodeStyleChecker.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;
using namespace tooling;

static llvm::cl::OptionCategory CSCCategory("code-style-checker options");

int main(int Argc, const char **Argv) {
  auto OptParser = CommonOptionsParser::create(Argc, Argv, CSCCategory);
  if (!OptParser) {
    llvm::errs() << "Problem constructing CommonOptionsParser "
                 << OptParser.takeError();
    return EXIT_FAILURE;
  }

  ClangTool Tool(OptParser->getCompilations(), OptParser->getSourcePathList());
  return Tool.run(newFrontendActionFactory<CodeStyleCheckerAction>().get());
}
