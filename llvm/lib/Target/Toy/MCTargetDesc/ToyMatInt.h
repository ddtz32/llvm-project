#pragma once

#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include <cstdint>

namespace llvm {

namespace ToyMatInt {

enum class OpcodeKind {
  RegImm, // ADDI, SLLI
  Imm,    // LUI
};

class Inst {
  unsigned Opcode;
  int Imm;

public:
  Inst(unsigned Opcode, int64_t Imm) : Opcode(Opcode), Imm(Imm) {
    assert(this->Imm == Imm && "truncated");
  }

  unsigned getOpcode() const { return Opcode; }
  int64_t getImm() const { return Imm; }
  OpcodeKind getOpcodeKind() const;
};

using InstSeq = SmallVector<Inst, 8>;

InstSeq generateInstSeq(int64_t Imm, const MCSubtargetInfo &STI);

void generateMCInstSeq(int64_t Imm, const MCSubtargetInfo &STI,
                       MCRegister DestReg, SmallVectorImpl<MCInst> &Insts);

} // namespace ToyMatInt

} // namespace llvm
