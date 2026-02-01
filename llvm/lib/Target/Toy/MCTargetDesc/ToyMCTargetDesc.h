#pragma once

namespace llvm {

class MCCodeEmitter;
class MCInstrInfo;
class MCContext;

MCCodeEmitter *createToyMCCodeEmitter(const MCInstrInfo &MCII, MCContext &Ctx);

} // namespace llvm

#define GET_REGINFO_ENUM
#include "ToyGenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#include "ToyGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "ToyGenSubtargetInfo.inc"