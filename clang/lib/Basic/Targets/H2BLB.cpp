#include "H2BLB.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetBuiltins.h"

using namespace clang;
using namespace clang::targets;

static constexpr int NumH2BLBBuiltins =
    H2BLB::LastTSBuiltin - Builtin::FirstTSBuiltin;

#define GET_BUILTIN_STR_TABLE
#include "clang/Basic/BuiltinsH2BLB.inc"
#undef GET_BUILTIN_STR_TABLE

static constexpr Builtin::Info BuiltinInfos[] = {
#define GET_BUILTIN_INFOS
#include "clang/Basic/BuiltinsH2BLB.inc"
#undef GET_BUILTIN_INFOS
};

H2BLBTargetInfo::H2BLBTargetInfo(const llvm::Triple &T) : TargetInfo(T) {
  PointerWidth = PointerAlign = 16;
  IntPtrType = SignedShort;
  PtrDiffType = SignedShort;
  SizeType = UnsignedShort;
  LongLongAlign = 32;
  resetDataLayout();
}

void H2BLBTargetInfo::getTargetDefines(const LangOptions &Opts,
                                       MacroBuilder &Builder) const {
  Builder.defineMacro("__H2BLB__");
}

llvm::SmallVector<Builtin::InfosShard>
H2BLBTargetInfo::getTargetBuiltins() const {
  return {{&BuiltinStrings, BuiltinInfos}};
}

TargetInfo::BuiltinVaListKind H2BLBTargetInfo::getBuiltinVaListKind() const {
  return {};
}

bool H2BLBTargetInfo::validateAsmConstraint(
    const char *&Name, TargetInfo::ConstraintInfo &info) const {
  return false;
}

std::string_view H2BLBTargetInfo::getClobbers() const { return {}; }

ArrayRef<const char *> H2BLBTargetInfo::getGCCRegNames() const { return {}; }

ArrayRef<TargetInfo::GCCRegAlias> H2BLBTargetInfo::getGCCRegAliases() const {
  return {};
}
