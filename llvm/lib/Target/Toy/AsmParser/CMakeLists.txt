add_llvm_component_library(LLVMToyAsmParser
  ToyAsmParser.cpp

  LINK_COMPONENTS
  MC
  MCParser
  Support
  ToyDesc
  ToyInfo

  ADD_TO_COMPONENT
  Toy
  )
