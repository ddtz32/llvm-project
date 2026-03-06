#pragma once

#include "llvm/MC/MCAsmBackend.h"
#include <cstdint>

namespace llvm {

class ToyAsmBackend : public MCAsmBackend {
public:
  ToyAsmBackend(bool IsLittleEndian);

  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override;

  // Determine if a relocation is required. In addition, apply `Value` to the
  // `Data` fragment at the specified fixup offset if applicable. `Data` points
  // to the first byte of the fixup offset, which may be at the content's end if
  // the fixup is zero-sized.
  void applyFixup(const MCFragment &, const MCFixup &, const MCValue &Target,
                  uint8_t *Data, uint64_t Value, bool IsResolved) override;

  /// Write an (optimal) nop sequence of Count bytes to the given output. If the
  /// target cannot generate such a sequence, it should return an error.
  ///
  /// \return - True on success.
  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override;
};

} // namespace llvm
