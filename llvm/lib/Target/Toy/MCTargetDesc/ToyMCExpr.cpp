#include "ToyMCAsmInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/BinaryFormat/ELF.h"

using namespace llvm;

Toy::Specifier Toy::parseSpeciferName(StringRef Name) {
  return StringSwitch<Toy::Specifier>(Name)
      .Case("lo", ELF::R_TOY_LO12)
      .Case("hi", ELF::R_TOY_HI20)
      .Case("pcrel_lo", ELF::R_TOY_PCREL_LO12)
      .Case("pcrel_hi", ELF::R_TOY_PCREL_HI20)
      .Default(ELF::R_TOY_NONE);
}

StringRef Toy::getSpecifierName(Specifier Kind) {
  switch (Kind) {
  case ELF::R_TOY_NONE:
    llvm_unreachable("not used as %specifier");
  case ELF::R_TOY_LO12:
    return "lo";
  case ELF::R_TOY_HI20:
    return "hi";
  case ELF::R_TOY_PCREL_LO12:
    return "pcrel_lo";
  case ELF::R_TOY_PCREL_HI20:
    return "pcrel_hi";
  }
  llvm_unreachable("invalid ELF symbol kind");
}
