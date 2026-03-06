#include "ToyAsmBackend.h"
#include "ToyMCTargetDesc.h"
#include "llvm/MC//MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdint>

using namespace llvm;

ToyAsmBackend::ToyAsmBackend(bool IsLittleEndian)
    : MCAsmBackend(IsLittleEndian ? endianness::little : endianness::big) {}

std::unique_ptr<MCObjectTargetWriter>
ToyAsmBackend::createObjectTargetWriter() const {
  return createToyELFObjectWriter();
}

// Determine if a relocation is required. In addition, apply `Value` to the
// `Data` fragment at the specified fixup offset if applicable. `Data` points
// to the first byte of the fixup offset, which may be at the content's end if
// the fixup is zero-sized.
void ToyAsmBackend::applyFixup(const MCFragment &, const MCFixup &,
                               const MCValue &Target, uint8_t *Data,
                               uint64_t Value, bool IsResolved) {
  llvm_unreachable("TODO");
}

/// Write an (optimal) nop sequence of Count bytes to the given output. If the
/// target cannot generate such a sequence, it should return an error.
///
/// \return - True on success.
bool ToyAsmBackend::writeNopData(raw_ostream &OS, uint64_t Count,
                                 const MCSubtargetInfo *STI) const {
  for (uint64_t Idx = 0; Idx < Count % 4; Idx++)
    OS.write("\0", 1);

  // The nap is addi x0, x0, 0. We cannot access instruction encoding
  // information here, so just write binary encoding here
  for (uint64_t Idx = 0; Idx < Count / 4; Idx++)
    OS.write("\x13\0\0\0", 4);

  return true;
};

MCAsmBackend *llvm::createToyAsmBackend(const Target &T,
                                        const MCSubtargetInfo &STI,
                                        const MCRegisterInfo &MRI,
                                        const MCTargetOptions &Options) {
  const Triple &TT = STI.getTargetTriple();
  return new ToyAsmBackend(TT.isLittleEndian());
}
