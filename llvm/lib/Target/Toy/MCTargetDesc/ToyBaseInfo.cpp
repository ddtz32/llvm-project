#include "ToyBaseInfo.h"
#include "llvm/ADT/ArrayRef.h"

namespace llvm {

namespace ToyInsnOpcode {
#define GET_ToyOpcodesList_IMPL
#include "ToyGenSearchableTables.inc"
} // namespace ToyInsnOpcode

namespace ToySysReg {
#define GET_SysRegsList_IMPL
#include "ToyGenSearchableTables.inc"
} // namespace ToySysReg

} // namespace llvm
