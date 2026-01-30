#include "ToyInstPrinter.h"
#include "ToyMCTargetDesc.h"
#include "llvm/MC/MCInst.h"

using namespace llvm;

#include "ToyGenAsmWriter.inc"

ToyInstPrinter::ToyInstPrinter(const MCAsmInfo &MAI, const MCInstrInfo &MII,
                               const MCRegisterInfo &MRI)
    : MCInstPrinter(MAI, MII, MRI) {}

bool ToyInstPrinter::applyTargetSpecificCLOption(StringRef Opt) {
  if (Opt == "no-aliases") {
    PrintAliases = false;
    return true;
  }

  return false;
}

void ToyInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                               StringRef Annot, const MCSubtargetInfo &STI,
                               raw_ostream &OS) {
  llvm_unreachable("TODO");
}

void ToyInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                  raw_ostream &O) {
  llvm_unreachable("TODO");
}
