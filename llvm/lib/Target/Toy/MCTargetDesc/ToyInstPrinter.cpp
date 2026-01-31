#include "ToyInstPrinter.h"
#include "ToyMCTargetDesc.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

static cl::opt<bool> EmitX8AsFp("toy-emit-x8-as-fp",
                                cl::desc("Emit x8 as fp instead of s0"),
                                cl::init(false), cl::Hidden);

// print ABI name by default, otherwise print register name
static bool ArchRegNames = false;

#include "ToyGenAsmWriter.inc"

ToyInstPrinter::ToyInstPrinter(const MCAsmInfo &MAI, const MCInstrInfo &MII,
                               const MCRegisterInfo &MRI)
    : MCInstPrinter(MAI, MII, MRI) {}

bool ToyInstPrinter::applyTargetSpecificCLOption(StringRef Opt) {
  if (Opt == "no-aliases") {
    PrintAliases = false;
    return true;
  }
  if (Opt == "numeric") {
    ArchRegNames = true;
    return true;
  }
  if (Opt == "emit-x8-as-fp") {
    if (!ArchRegNames)
      EmitX8AsFp = true;
    return true;
  }

  return false;
}

const char *ToyInstPrinter::getRegisterName(MCRegister Reg) {
  if (!ArchRegNames && EmitX8AsFp && Reg == Toy::X8)
    return "fp";
  return getRegisterName(Reg,
                         ArchRegNames ? Toy::NoRegAltName : Toy::ABIRegAltName);
}

void ToyInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                               StringRef Annot, const MCSubtargetInfo &STI,
                               raw_ostream &OS) {
  printInstruction(MI, Address, OS);
  printAnnotation(OS, Annot);
}

void ToyInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                  raw_ostream &OS) {
  const MCOperand &MO = MI->getOperand(OpNo);
  if (MO.isReg())
    return printRegName(OS, MO.getReg());

  if (MO.isImm()) {
    markup(OS, Markup::Immediate) << formatImm(MO.getImm());
    return;
  }

  llvm_unreachable("TODO");
}

void ToyInstPrinter::printRegName(raw_ostream &OS, MCRegister Reg) {
  markup(OS, Markup::Register) << getRegisterName(Reg);
}