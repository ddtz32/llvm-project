add_llvm_component_library(LLVMToyDesc
  ToyInstPrinter.cpp
  ToyMCTargetDesc.cpp

  LINK_COMPONENTS
  MC
  Support
  TargetParser
  ToyInfo

  ADD_TO_COMPONENT
  Toy
  )
