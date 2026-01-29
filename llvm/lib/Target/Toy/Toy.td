include "llvm/Target/Target.td"

include "ToyFeatures.td"
include "ToyProcessors.td"

include "ToyRegisterInfo.td"

include "ToyInstrInfo.td"

def ToyInstrInfo : InstrInfo {
  let guessInstructionProperties = 0;
}

// To handle the following error, add this `defm`
// error: missing target override for pseudoinstruction using PointerLikeRegClass
defm : RemapAllTargetPseudoPointerOperands<GPR>;

def Toy : Target {
  let InstructionSet = ToyInstrInfo;
}