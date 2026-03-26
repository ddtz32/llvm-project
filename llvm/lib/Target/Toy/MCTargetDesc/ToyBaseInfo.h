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

namespace ToySysReg {
#define GET_SysRegEncodings_DECL
#include "ToyGenSearchableTables.inc"

struct SysReg {
  const char Name[10];
  unsigned Encoding;

  SysRegEncodings getEncoding () const {
    return static_cast<SysRegEncodings>(Encoding);
  }
};

#define GET_SysRegsList_DECL
#include "ToyGenSearchableTables.inc"
} // namespace ToySysReg

} // namespace llvm
