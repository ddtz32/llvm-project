#pragma once

#include "clang/Basic/TargetInfo.h"

namespace clang {
namespace targets {

class H2BLBTargetInfo final : public TargetInfo {
public:
  H2BLBTargetInfo(const llvm::Triple &T);

  // implement pure virtual functions
  void getTargetDefines(const LangOptions &Opts,
                        MacroBuilder &Builder) const override;

  llvm::SmallVector<Builtin::InfosShard> getTargetBuiltins() const override;

  BuiltinVaListKind getBuiltinVaListKind() const override;

  bool validateAsmConstraint(const char *&Name,
                             TargetInfo::ConstraintInfo &info) const override;

  std::string_view getClobbers() const override;

  ArrayRef<const char *> getGCCRegNames() const override;

  ArrayRef<GCCRegAlias> getGCCRegAliases() const override;
};

} // namespace targets
} // namespace clang
