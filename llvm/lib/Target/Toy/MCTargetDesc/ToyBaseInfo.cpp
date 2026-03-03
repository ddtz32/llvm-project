#include "ToyBaseInfo.h"
#include "llvm/ADT/ArrayRef.h"

namespace llvm {

namespace ToyInsnOpcode {

#define GET_ToyOpcodesList_IMPL
#include "ToyGenSearchableTables.inc"
} // namespace ToyInsnOpcode

} // namespace llvm
