#include "ToyMCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/ErrorHandling.h"
#include <memory>

using namespace llvm;

namespace {
class ToyELFObjectWriter : public MCELFObjectTargetWriter {
public:
  ToyELFObjectWriter();

  unsigned getRelocType(const MCFixup &Fixup, const MCValue &Target,
                        bool IsPCRel) const override;
};
} // namespace
ToyELFObjectWriter::ToyELFObjectWriter()
    : MCELFObjectTargetWriter(false, 0, ELF::EM_TOY, true) {}

unsigned ToyELFObjectWriter::getRelocType(const MCFixup &Fixup,
                                          const MCValue &Target,
                                          bool IsPCRel) const {
  llvm_unreachable("TODO");
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createToyELFObjectWriter() {
  return std::make_unique<ToyELFObjectWriter>();
}
