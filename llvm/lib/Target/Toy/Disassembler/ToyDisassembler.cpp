#include "MCTargetDesc/ToyMCTargetDesc.h"
#include "TargetInfo/ToyTargetInfo.h"
#include "llvm/MC/MCDecoder.h"
#include "llvm/MC/MCDecoderOps.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include <cstdint>

using namespace llvm;
using namespace llvm::MCD;

using DecodeStatus = MCDisassembler::DecodeStatus;

#define DEBUG_TYPE "toy-disassembler"

namespace {

class ToyDisassembler : public MCDisassembler {
public:
  ToyDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx);

  /// Returns the disassembly of a single instruction.
  ///
  /// \param Instr    - An MCInst to populate with the contents of the
  ///                   instruction.
  /// \param Size     - A value to populate with the size of the instruction, or
  ///                   the number of bytes consumed while attempting to decode
  ///                   an invalid instruction.
  /// \param Address  - The address, in the memory space of region, of the first
  ///                   byte of the instruction.
  /// \param Bytes    - A reference to the actual bytes of the instruction.
  /// \param CStream  - The stream to print comments and annotations on.
  /// \return         - MCDisassembler::Success if the instruction is valid,
  ///                   MCDisassembler::SoftFail if the instruction was
  ///                                            disassemblable but invalid,
  ///                   MCDisassembler::Fail if the instruction was invalid.
  DecodeStatus getInstruction(MCInst &MI, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &CStream) const override;
};

DecodeStatus DecodeGPRRegisterClass(MCInst &MI, unsigned RegNo,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder) {
  if (RegNo > 32)
    return DecodeStatus::Fail;

  // TODO: Is is safe to add RegNo to X0?
  MCRegister Reg = Toy::X0 + RegNo;
  MI.addOperand(MCOperand::createReg(Reg));
  return DecodeStatus::Success;
}

template <unsigned N>
DecodeStatus decodeSImmOperand(MCInst &MI, unsigned Imm, uint64_t Address,
                               const MCDisassembler *Decoder) {
  assert(isUInt<N>(Imm) && "Invalid immediate");
  MI.addOperand(MCOperand::createImm(SignExtend64<N>(Imm)));
  return DecodeStatus::Success;
}

template <unsigned T, unsigned N>
DecodeStatus decodeSImmOperandAndLslN(MCInst &MI, unsigned Imm, uint64_t Address,
                                     const MCDisassembler *Decoder) {
  assert(isUInt<T - N>(Imm) && "Invalid immediate");
  MI.addOperand(MCOperand::createImm(SignExtend64<T>(Imm << N)));
  return DecodeStatus::Success;
}

MCDisassembler *createToyDisassembler(const Target &T,
                                      const MCSubtargetInfo &STI,
                                      MCContext &Ctx) {
  return new ToyDisassembler(STI, Ctx);
}

} // namespace

#include "ToyGenDisassembler.inc"

ToyDisassembler::ToyDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx)
    : MCDisassembler(STI, Ctx) {}

DecodeStatus ToyDisassembler::getInstruction(MCInst &MI, uint64_t &Size,
                                             ArrayRef<uint8_t> Bytes,
                                             uint64_t Address,
                                             raw_ostream &CStream) const {
  if (Bytes.size() < 4) {
    Size = 0;
    return DecodeStatus::Fail;
  }

  Size = 4;
  unsigned Inst = support::endian::read32le(Bytes.data());
  return decodeInstruction(DecoderTable32, MI, Inst, Address, this, STI);
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void
LLVMInitializeToyDisassembler() {
  TargetRegistry::RegisterMCDisassembler(getTheToy32Target(),
                                         createToyDisassembler);
  TargetRegistry::RegisterMCDisassembler(getTheToy64Target(),
                                         createToyDisassembler);
}
