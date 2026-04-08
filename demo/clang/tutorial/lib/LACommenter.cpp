#include "LACommenter.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;
using namespace ast_matchers;

void LACommenterMatcher::run(
    const clang::ast_matchers::MatchFinder::MatchResult &Result) {
  ASTContext *Ctx = Result.Context;
  const auto *CalleeDecl = Result.Nodes.getNodeAs<FunctionDecl>("callee");
  const auto *Caller = Result.Nodes.getNodeAs<CallExpr>("caller");

  // CalleeDecl->dump();
  // Caller->dump();

  assert(CalleeDecl && Caller && "callee and caller must be non-null");

  if (CalleeDecl->param_empty())
    return;

  for (const auto [Arg, ParamDecl] :
       llvm::zip_first(Caller->arguments(), CalleeDecl->parameters())) {
    const Expr *E = Arg->IgnoreParenCasts();
    if (!dyn_cast<CXXBoolLiteralExpr>(E) && !dyn_cast<CharacterLiteral>(E) &&
        !dyn_cast<StringLiteral>(E) && !dyn_cast<IntegerLiteral>(E) &&
        !dyn_cast<FloatingLiteral>(E))
      continue;

    auto ParamLoc = Ctx->getFullLoc(ParamDecl->getBeginLoc());
    auto ArgLoc = Ctx->getFullLoc(Arg->getBeginLoc());
    if (ParamLoc.isValid() && !ParamDecl->getDeclName().isEmpty() &&
        ArgLoc.isValid() && EditedLocations.insert(ArgLoc).second)
      Rewriter.InsertText(
          ArgLoc,
          (llvm::Twine("/*") + ParamDecl->getDeclName().getAsString() + "*/")
              .str());
  }
}

void LACommenterConsumer::HandleTranslationUnit(ASTContext &Ctx) {
  StatementMatcher Matcher =
      callExpr(callee(functionDecl(unless(isVariadic())).bind("callee")),
               anyOf(hasAnyArgument(ignoringParenCasts(cxxBoolLiteral())),
                     hasAnyArgument(ignoringParenCasts(characterLiteral())),
                     hasAnyArgument(ignoringParenCasts(stringLiteral())),
                     hasAnyArgument(ignoringParenCasts(integerLiteral())),
                     hasAnyArgument(ignoringParenCasts(floatLiteral()))))
          .bind("caller");

  MatchFinder Finder;
  LACommenterMatcher LACHandler(Rewriter);
  Finder.addMatcher(Matcher, &LACHandler);
  Finder.matchAST(Ctx);

  Rewriter.getEditBuffer(Rewriter.getSourceMgr().getMainFileID())
      .write(llvm::outs());
}

std::unique_ptr<clang::ASTConsumer>
LACommenterAction::CreateASTConsumer(clang::CompilerInstance &CI,
                                     llvm::StringRef InFile) {
  Rewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
  return std::make_unique<LACommenterConsumer>(Rewriter);
}

static FrontendPluginRegistry::Add<LACommenterAction>
    X("lac", "literal argument commenter");
