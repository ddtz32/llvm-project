add_llvm_component_group(Toy)

set(LLVM_TARGET_DEFINITIONS Toy.td)

tablegen(LLVM ToyGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM ToyGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM ToyGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM ToyGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM ToyGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM ToyGenMCCodeEmitter.inc -gen-emitter)

add_public_tablegen_target(ToyCommonTableGen)

add_llvm_target(ToyCodeGen
  ToyTargetMachine.cpp

  LINK_COMPONENTS
  CodeGen
  MC
  Support
  Target
  TargetParser
  ToyInfo

  ADD_TO_COMPONENT
  Toy
  )

add_subdirectory(AsmParser)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
