#pragma once

#include "llvm/MC/MCObjectWriter.h"
#include <cstdint>
#include <memory>
namespace llvm {

class MCCodeEmitter;
class MCInstrInfo;
class MCContext;
class MCAsmBackend;
class MCSubtargetInfo;
class MCRegisterInfo;
class MCTargetOptions;
class MCObjectTargetWriter;
class Target;

MCCodeEmitter *createToyMCCodeEmitter(const MCInstrInfo &MCII, MCContext &Ctx);

MCAsmBackend *createToyAsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                  const MCRegisterInfo &MRI,
                                  const MCTargetOptions &Options);

std::unique_ptr<MCObjectTargetWriter> createToyELFObjectWriter();
} // namespace llvm

#define GET_REGINFO_ENUM
#include "ToyGenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#include "ToyGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "ToyGenSubtargetInfo.inc"
