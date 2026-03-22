#include "ToyMatInt.h"
#include "ToyMCTargetDesc.h"
#include "llvm/ADT/bit.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/Support/MathExtras.h"
#include <cstdint>

using namespace llvm;
using namespace llvm::ToyMatInt;

OpcodeKind Inst::getOpcodeKind() const {
  switch (getOpcode()) {
  default:
    llvm_unreachable("Unexpected opcode!");
  case Toy::ADDI:
  case Toy::SLLI:
    return OpcodeKind::RegImm;
  case Toy::LUI:
    return OpcodeKind::Imm;
  }
}

static void generateInstSeqImpl(int64_t Imm, const MCSubtargetInfo &STI,
                                InstSeq &Seq) {
  // bool IsToy64 = STI.hasFeature(Toy::Feature64Bit);

  int64_t Hi20 = ((Imm + 0x800) >> 12) & 0xFFFFF;
  int64_t Lo12 = SignExtend64<12>(Imm);
  if (Hi20)
    Seq.emplace_back(Toy::LUI, Hi20);
  if (Lo12)
    Seq.emplace_back(Toy::ADDI, Lo12);
}

InstSeq ToyMatInt::generateInstSeq(int64_t Imm, const MCSubtargetInfo &STI) {
  assert(!isInt<12>(Imm) && "simm12 should be match to li simm12");
  assert(isInt<32>(Imm) && "Toy32 only support 32 bit immediate");

  InstSeq Seq;
  generateInstSeqImpl(Imm, STI, Seq);

  if ((Imm & 0xFFF) != 0 && (Imm & 1) == 0 && Seq.size() == 2) {
    unsigned TrailingZeros = countr_zero(static_cast<uint64_t>(Imm));
    int64_t ShiftedImm = Imm >> TrailingZeros;
    InstSeq TmpSeq;
    generateInstSeqImpl(ShiftedImm, STI, TmpSeq);
    if (TmpSeq.size() + 1 <= Seq.size()) {
      TmpSeq.emplace_back(Toy::SLLI, TrailingZeros);
      Seq = TmpSeq;
    }
  }
  return Seq;
}

void ToyMatInt::generateMCInstSeq(int64_t Imm, const MCSubtargetInfo &STI,
                                  MCRegister DestReg,
                                  SmallVectorImpl<MCInst> &Insts) {
  InstSeq Seq = generateInstSeq(Imm, STI);
  MCRegister SrcReg = Toy::X0;
  for (auto &Inst : Seq) {
    switch (Inst.getOpcodeKind()) {
    case OpcodeKind::RegImm:
      Insts.push_back(MCInstBuilder(Inst.getOpcode())
                          .addReg(DestReg)
                          .addReg(SrcReg)
                          .addImm(Inst.getImm()));
      break;
    case OpcodeKind::Imm:
      Insts.push_back(MCInstBuilder(Inst.getOpcode())
                          .addReg(DestReg)
                          .addImm(Inst.getImm()));
      break;
    }
    SrcReg = DestReg;
  }
}
