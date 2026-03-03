add_llvm_component_library(LLVMToyDesc
  ToyBaseInfo.cpp
  ToyInstPrinter.cpp
  ToyMCCodeEmitter.cpp
  ToyMCTargetDesc.cpp

  LINK_COMPONENTS
  MC
  Support
  TargetParser
  ToyInfo

  ADD_TO_COMPONENT
  Toy
  )
