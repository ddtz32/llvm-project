#pragma once

#include "llvm/MC/MCAsmInfoELF.h"
#include <cstdint>

namespace llvm {
class StringRef;

class ToyMCAsmInfo : public MCAsmInfoELF {
public:
  explicit ToyMCAsmInfo() = default;

  void printSpecifierExpr(raw_ostream &OS,
                          const MCSpecifierExpr &Expr) const override;
};

namespace Toy {

using Specifier = uint16_t;

Specifier parseSpeciferName(StringRef Name);
StringRef getSpecifierName(Specifier Kind);

} // namespace Toy

} // namespace llvm
