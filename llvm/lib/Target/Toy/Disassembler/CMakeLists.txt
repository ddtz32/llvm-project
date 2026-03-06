add_llvm_component_library(LLVMToyDisassembler
  ToyDisassembler.cpp

  LINK_COMPONENTS
  MCDisassembler
  Support
  ToyInfo

  ADD_TO_COMPONENT
  Toy
  )
