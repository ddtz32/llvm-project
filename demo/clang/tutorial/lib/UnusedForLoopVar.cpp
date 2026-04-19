#include "UnusedForLoopVar.h"
#include "clang/AST/StmtCXX.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;
using namespace ast_matchers;

namespace {
void handleRegularForLoop(const MatchFinder::MatchResult &Result,
                          ASTContext &Ctx) {
  const auto *RegularForLoop =
      Result.Nodes.getNodeAs<ForStmt>("RegularForLoop");
  const auto *RegularForLoopVar =
      Result.Nodes.getNodeAs<DeclStmt>("RegularForLoopVar");

  // RegularForLoop->dump();

  if (!Ctx.getSourceManager().isInMainFile(RegularForLoop->getForLoc()))
    return;

  for (auto *LoopVar : RegularForLoopVar->decls()) {
    if (LoopVar->isUsed())
      continue;

    assert(llvm::isa<VarDecl>(LoopVar));
    auto *LoopNamedVar = cast<VarDecl>(LoopVar);

    auto &D = Ctx.getDiagnostics();
    unsigned DiagID = D.getCustomDiagID(
        DiagnosticsEngine::Warning,
        "(AST Matcher) regular for-loop variable '%0' not used");
    auto DB = D.Report(LoopVar->getLocation(), DiagID)
              << LoopNamedVar->getNameAsString();
    DB.AddSourceRange(CharSourceRange::getCharRange(LoopVar->getSourceRange()));
  }
}

void handleRangeForLoop(const MatchFinder::MatchResult &Result,
                        ASTContext &Ctx) {
  const auto *RangeForLoop =
      Result.Nodes.getNodeAs<CXXForRangeStmt>("RangeForLoop");
  const auto *LoopVar = Result.Nodes.getNodeAs<VarDecl>("RangeForLoopVar");

  // RangeForLoop->dump();

  if (!Ctx.getSourceManager().isInMainFile(RangeForLoop->getForLoc()))
    return;

  if (LoopVar->isUsed())
    return;

  assert(llvm::isa<NamedDecl>(LoopVar));
  auto *LoopNamedVar = cast<NamedDecl>(LoopVar);

  auto &D = Ctx.getDiagnostics();
  unsigned DiagID =
      D.getCustomDiagID(DiagnosticsEngine::Warning,
                        "(AST Matcher) range for-loop variable '%0' not used");
  auto DB = D.Report(LoopVar->getLocation(), DiagID)
            << LoopNamedVar->getNameAsString();
  DB.AddSourceRange(CharSourceRange::getCharRange(LoopVar->getSourceRange()));
}
} // namespace

void UnusedForLoopVarMatcher::run(const MatchFinder::MatchResult &Result) {
  ASTContext &Ctx = *Result.Context;

  if (Result.Nodes.getNodeAs<ForStmt>("RegularForLoop"))
    return handleRegularForLoop(Result, Ctx);

  if (Result.Nodes.getNodeAs<CXXForRangeStmt>("RangeForLoop"))
    return handleRangeForLoop(Result, Ctx);
}

bool UnusedForLoopVarVisitor::TraverseForStmt(ForStmt *ForLoop) {
  auto *InitStmt = dyn_cast<DeclStmt>(ForLoop->getInit());
  if (!InitStmt)
    return true;

  for (auto *LoopVar : InitStmt->decls()) {
    assert(llvm::isa<VarDecl>(LoopVar));
    auto *LoopNamedVar = cast<VarDecl>(LoopVar);
    if (!llvm::StringRef("unused").equals_insensitive(
            LoopNamedVar->getNameAsString()))
      LoopVars.insert(LoopNamedVar);
  }

  if (LoopVars.empty())
    return true;

  if (!RecursiveASTVisitor::TraverseStmt(ForLoop->getBody()))
    return false;

  std::set<VarDecl *> UnusedLoopVars;
  std::set_difference(LoopVars.begin(), LoopVars.end(), UsedLoopVars.begin(),
                      UsedLoopVars.end(),
                      std::inserter(UnusedLoopVars, UnusedLoopVars.begin()));
  for (auto *LoopVar : UnusedLoopVars) {
    auto &D = Ctx.getDiagnostics();
    unsigned DiagID = D.getCustomDiagID(
        DiagnosticsEngine::Warning,
        "(Recursive AST Visitor) regular for-loop variable '%0' not used");
    auto DB = D.Report(LoopVar->getLocation(), DiagID)
              << LoopVar->getNameAsString();
    DB.AddSourceRange(CharSourceRange::getCharRange(LoopVar->getSourceRange()));
  }
  LoopVars.clear();
  UsedLoopVars.clear();

  return true;
}

bool UnusedForLoopVarVisitor::VisitDeclRefExpr(clang::DeclRefExpr *DeclRef) {
  auto *LoopVar = llvm::dyn_cast<VarDecl>(DeclRef->getDecl());
  if (!LoopVar || LoopVars.count(LoopVar) == 0)
    return true;
  UsedLoopVars.insert(LoopVar);
  return true;
}

void UnusedForLoopVarConsumer::HandleTranslationUnit(clang::ASTContext &Ctx) {
  MatchFinder Finder;
  UnusedForLoopVarMatcher Matcher;

  auto MatchName = unless(matchesName("unused", llvm::Regex::IgnoreCase));
  StatementMatcher RangeForMatcher =
      cxxForRangeStmt(
          hasLoopVariable(varDecl(MatchName).bind("RangeForLoopVar")))
          .bind("RangeForLoop");
  Finder.addMatcher(RangeForMatcher, &Matcher);

  StatementMatcher RegularForMatcher =
      forStmt(
          hasLoopInit(
              declStmt(forEach(varDecl(MatchName))).bind("RegularForLoopVar")))
          .bind("RegularForLoop");
  Finder.addMatcher(RegularForMatcher, &Matcher);

  Finder.matchAST(Ctx);

  UnusedForLoopVarVisitor Visitor(Ctx);
  for (auto *Decl : Ctx.getTranslationUnitDecl()->decls())
    if (Ctx.getSourceManager().isInMainFile(Decl->getLocation()))
      Visitor.TraverseDecl(Decl);
}

static FrontendPluginRegistry::Add<UnusedForLoopVarAction>
    X("uflv", "find unused for-loop variables");
