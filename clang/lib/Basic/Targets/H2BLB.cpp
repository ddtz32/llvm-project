#include "H2BLB.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetBuiltins.h"

using namespace clang;
using namespace clang::targets;

static constexpr int NumH2BLBBuiltins =
    H2BLB::LastTSBuiltin - Builtin::FirstTSBuiltin;

static constexpr llvm::StringTable BuiltinH2BLBStrings =
    CLANG_BUILTIN_STR_TABLE_START
#define BUILTIN CLANG_BUILTIN_STR_TABLE
#include "clang/Basic/BuiltinsH2BLB.def"
    ;

static constexpr auto BuiltinH2BLBInfos = Builtin::MakeInfos<NumH2BLBBuiltins>({
#define BUILTIN CLANG_BUILTIN_ENTRY
#include "clang/Basic/BuiltinsH2BLB.def"
});

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
  return {{&BuiltinH2BLBStrings, BuiltinH2BLBInfos}};
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
