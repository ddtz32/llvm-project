add_llvm_component_library(LLVMToyDesc
  ToyAsmBackend.cpp
  ToyBaseInfo.cpp
  ToyELFObjectWriter.cpp
  ToyInstPrinter.cpp
  ToyMCAsmInfo.cpp
  ToyMCCodeEmitter.cpp
  ToyMCExpr.cpp
  ToyMCTargetDesc.cpp

  LINK_COMPONENTS
  MC
  Support
  TargetParser
  ToyInfo

  ADD_TO_COMPONENT
  Toy
  )
