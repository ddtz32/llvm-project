#include "ToyMCAsmInfo.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

void ToyMCAsmInfo::printSpecifierExpr(raw_ostream &OS,
                                      const MCSpecifierExpr &Expr) const {
  Toy::Specifier Spec = Expr.getSpecifier();
  bool HasSpecifier = Spec != ELF::R_TOY_NONE;
  if (HasSpecifier)
    OS << '%' << Toy::getSpecifierName(Spec) << '(';
  printExpr(OS, *Expr.getSubExpr());
  if (HasSpecifier)
    OS << ')';
}
