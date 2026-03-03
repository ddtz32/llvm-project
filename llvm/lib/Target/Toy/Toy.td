include "llvm/Target/Target.td"

include "llvm/TableGen/SearchableTable.td"

include "ToyFeatures.td"
include "ToyProcessors.td"

include "ToyRegisterInfo.td"

include "ToyInstrInfo.td"

def ToyInstrInfo : InstrInfo {
  let guessInstructionProperties = 0;
}

def ToyAsmParser : AsmParser {
  let ShouldEmitMatchRegisterAltName = 1;
}

def ToyAsmParserVariant : AsmParserVariant {
  let TokenizingCharacters = "()[]*!";
}

// To handle the following error, add this `defm`
// error: missing target override for pseudoinstruction using PointerLikeRegClass
defm : RemapAllTargetPseudoPointerOperands<GPR>;

def Toy : Target {
  let InstructionSet = ToyInstrInfo;
  let AssemblyParsers = [ToyAsmParser];
  let AssemblyParserVariants = [ToyAsmParserVariant];
}
