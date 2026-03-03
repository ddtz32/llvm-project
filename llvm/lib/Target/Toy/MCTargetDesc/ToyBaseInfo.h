#pragma once

#include "llvm/ADT/StringRef.h"

namespace llvm {

enum class ToyFenceField {
  I = 8,
  O = 4,
  R = 2,
  W = 1,
};

namespace ToyInsnOpcode {

struct ToyOpcode {
  char Name[10];
  uint8_t Value;
};

#define GET_ToyOpcodesList_DECL
#include "ToyGenSearchableTables.inc"
} // namespace ToyInsnOpcode

} // namespace llvm
