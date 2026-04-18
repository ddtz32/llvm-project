#include "Obfuscator.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;
using namespace ast_matchers;

namespace {
struct OperandInfo {
  std::string Operand;
  SourceRange Range;
};

OperandInfo extractOperandInfo(const MatchFinder::MatchResult &Result,
                               llvm::StringRef Name) {
  if (const auto *Operand = Result.Nodes.getNodeAs<DeclRefExpr>(Name)) {
    std::string Name =
        llvm::dyn_cast<DeclRefExpr>(Operand)->getDecl()->getNameAsString();
    SourceRange Range = Operand->getSourceRange();
    return {Name, Range};
  }

  if (const auto *Operand = Result.Nodes.getNodeAs<IntegerLiteral>(Name)) {
    std::string Number = std::to_string(Operand->getValue().getZExtValue());
    SourceRange Range = Operand->getSourceRange();
    return {Number, Range};
  }

  llvm_unreachable("Unsupported operand of '+' or '-'");
}
} // namespace

void ObfuscatorMatcherForAdd::run(const MatchFinder::MatchResult &Result) {
  auto LHS = extractOperandInfo(Result, "lhs");
  auto RHS = extractOperandInfo(Result, "rhs");

  Rewriter.ReplaceText(LHS.Range,
                       "(" + LHS.Operand + " ^ " + RHS.Operand + ")");
  Rewriter.ReplaceText(RHS.Range,
                       "2 * (" + LHS.Operand + " & " + RHS.Operand + ")");
}

void ObfuscatorMatcherForSub ::run(const MatchFinder::MatchResult &Result) {
  const auto &Sub = Result.Nodes.getNodeAs<BinaryOperator>("sub");

  auto LHS = extractOperandInfo(Result, "lhs");
  auto RHS = extractOperandInfo(Result, "rhs");

  Rewriter.ReplaceText(LHS.Range,
                       "(" + LHS.Operand + " + ~" + RHS.Operand + ")");
  Rewriter.ReplaceText(Sub->getOperatorLoc(), "+");
  Rewriter.ReplaceText(RHS.Range, "1");
}

void ObfuscatorConsumer ::HandleTranslationUnit(clang::ASTContext &Ctx) {
  StatementMatcher AddMatcher = binaryOperator(
      hasOperatorName("+"),
      hasLHS(anyOf(
          implicitCastExpr(declRefExpr(hasType(isSignedInteger())).bind("lhs")),
          integerLiteral().bind("lhs"))),
      hasRHS(anyOf(
          implicitCastExpr(declRefExpr(hasType(isSignedInteger())).bind("rhs")),
          integerLiteral().bind("rhs"))));

  StatementMatcher SubMatcher =
      binaryOperator(
          hasOperatorName("-"),
          hasLHS(anyOf(implicitCastExpr(
                           declRefExpr(hasType(isSignedInteger())).bind("lhs")),
                       integerLiteral().bind("lhs"))),
          hasRHS(anyOf(implicitCastExpr(
                           declRefExpr(hasType(isSignedInteger())).bind("rhs")),
                       integerLiteral().bind("rhs"))))
          .bind("sub");

  MatchFinder Finder;
  ObfuscatorMatcherForAdd AddHandler(Rewriter);
  ObfuscatorMatcherForSub SubHandler(Rewriter);
  Finder.addMatcher(AddMatcher, &AddHandler);
  Finder.addMatcher(SubMatcher, &SubHandler);
  Finder.matchAST(Ctx);
}

std::unique_ptr<clang::ASTConsumer>
ObfuscatorAction::CreateASTConsumer(clang::CompilerInstance &CI,
                                    llvm::StringRef InFile) {

  Rewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
  return std::make_unique<ObfuscatorConsumer>(Rewriter);
}

void ObfuscatorAction::EndSourceFileAction() {
  PluginASTAction::EndSourceFileAction();

  Rewriter.getEditBuffer(Rewriter.getSourceMgr().getMainFileID())
      .write(llvm::outs());
}

static FrontendPluginRegistry::Add<ObfuscatorAction>
    X("obfuscator", "obfuscator add and sub operator");
