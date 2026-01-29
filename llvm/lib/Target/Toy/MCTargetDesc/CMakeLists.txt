add_llvm_component_library(LLVMToyDesc
  ToyTargetDesc.cpp

  LINK_COMPONENTS
  MC
  Support
  TargetParser
  ToyInfo

  ADD_TO_COMPONENT
  Toy
  )
